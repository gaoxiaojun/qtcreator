/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "breakpoint.h"

#include "utils/qtcassert.h"

#include <QByteArray>
#include <QDebug>
#include <QFileInfo>

namespace Debugger {
namespace Internal {

//////////////////////////////////////////////////////////////////
//
// BreakpointModelId
//
//////////////////////////////////////////////////////////////////

/*!
    \class Debugger::Internal::ModelId

    This identifies a breakpoint in the \c BreakHandler. The
    major parts are strictly increasing over time.

    The minor part identifies a multiple breakpoint
    set for example by gdb in constructors.
*/


QDebug operator<<(QDebug d, const BreakpointModelId &id)
{
    d << qPrintable(id.toString());
    return d;
}

QByteArray BreakpointModelId::toByteArray() const
{
    if (!isValid())
        return "<invalid bkpt>";
    QByteArray ba = QByteArray::number(m_majorPart);
    if (isMinor()) {
        ba.append('.');
        ba.append(QByteArray::number(m_minorPart));
    }
    return ba;
}

QString BreakpointModelId::toString() const
{
    if (!isValid())
        return QLatin1String("<invalid bkpt>");
    if (isMinor())
        return QString::fromLatin1("%1.%2").arg(m_majorPart).arg(m_minorPart);
    return QString::number(m_majorPart);
}

BreakpointModelId BreakpointModelId::parent() const
{
    QTC_ASSERT(isMinor(), return BreakpointModelId());
    return BreakpointModelId(m_majorPart, 0);
}

BreakpointModelId BreakpointModelId::child(int row) const
{
    QTC_ASSERT(isMajor(), return BreakpointModelId());
    return BreakpointModelId(m_majorPart, row + 1);
}


//////////////////////////////////////////////////////////////////
//
// BreakpointResponseId
//
//////////////////////////////////////////////////////////////////

/*!
    \class Debugger::Internal::BreakpointResponseId

    This is what the external debuggers use to identify a breakpoint.
    It is only valid for one debugger run.

    In gdb, the breakpoint number is used, which is constant
    during a session. CDB's breakpoint numbers vary if breakpoints
    are deleted, so, the ID is used.
*/

BreakpointResponseId::BreakpointResponseId(const QByteArray &ba)
{
    int pos = ba.indexOf('.');
    if (pos == -1) {
        m_majorPart = ba.toInt();
        m_minorPart = 0;
    } else {
        m_majorPart = ba.left(pos).toInt();
        m_minorPart = ba.mid(pos + 1).toInt();
    }
}

QDebug operator<<(QDebug d, const BreakpointResponseId &id)
{
    d << qPrintable(id.toString());
    return d;
}

QByteArray BreakpointResponseId::toByteArray() const
{
    if (!isValid())
        return "<invalid bkpt>";
    QByteArray ba = QByteArray::number(m_majorPart);
    if (isMinor()) {
        ba.append('.');
        ba.append(QByteArray::number(m_minorPart));
    }
    return ba;
}

QString BreakpointResponseId::toString() const
{
    if (!isValid())
        return QLatin1String("<invalid bkpt>");
    if (isMinor())
        return QString::fromLatin1("%1.%2").arg(m_majorPart).arg(m_minorPart);
    return QString::number(m_majorPart);
}

BreakpointResponseId BreakpointResponseId::parent() const
{
    QTC_ASSERT(isMinor(), return BreakpointResponseId());
    return BreakpointResponseId(m_majorPart, 0);
}

BreakpointResponseId BreakpointResponseId::child(int row) const
{
    QTC_ASSERT(isMajor(), return BreakpointResponseId());
    return BreakpointResponseId(m_majorPart, row + 1);
}

//////////////////////////////////////////////////////////////////
//
// BreakpointParameters
//
//////////////////////////////////////////////////////////////////

/*!
    \class Debugger::Internal::BreakpointParameters

    Data type holding the parameters of a breakpoint.
*/

BreakpointParameters::BreakpointParameters(BreakpointType t)
  : type(t), enabled(true), pathUsage(BreakpointPathUsageEngineDefault),
    ignoreCount(0), lineNumber(0), address(0), size(0),
    bitpos(0), bitsize(0), threadSpec(-1),
    tracepoint(false)
{}

BreakpointParts BreakpointParameters::differencesTo
    (const BreakpointParameters &rhs) const
{
    BreakpointParts parts = BreakpointParts();
    if (type != rhs.type)
        parts |= TypePart;
    if (enabled != rhs.enabled)
        parts |= EnabledPart;
    if (pathUsage != rhs.pathUsage)
        parts |= PathUsagePart;
    if (fileName != rhs.fileName)
        parts |= FileAndLinePart;
    if (!conditionsMatch(rhs.condition))
        parts |= ConditionPart;
    if (ignoreCount != rhs.ignoreCount)
        parts |= IgnoreCountPart;
    if (lineNumber != rhs.lineNumber)
        parts |= FileAndLinePart;
    if (address != rhs.address)
        parts |= AddressPart;
    if (threadSpec != rhs.threadSpec)
        parts |= ThreadSpecPart;
    if (functionName != rhs.functionName)
        parts |= FunctionPart;
    if (tracepoint != rhs.tracepoint)
        parts |= TracePointPart;
    if (module != rhs.module)
        parts |= ModulePart;
    if (command != rhs.command)
        parts |= CommandPart;
    if (message != rhs.message)
        parts |= MessagePart;
    return parts;
}

bool BreakpointParameters::isValid() const
{
    switch (type) {
    case Debugger::Internal::BreakpointByFileAndLine:
        return !fileName.isEmpty() && lineNumber > 0;
    case Debugger::Internal::BreakpointByFunction:
        return !functionName.isEmpty();
    case Debugger::Internal::WatchpointAtAddress:
    case Debugger::Internal::BreakpointByAddress:
        return address != 0;
    case Debugger::Internal::BreakpointAtThrow:
    case Debugger::Internal::BreakpointAtCatch:
    case Debugger::Internal::BreakpointAtMain:
    case Debugger::Internal::BreakpointAtFork:
    case Debugger::Internal::BreakpointAtExec:
    case Debugger::Internal::BreakpointAtSysCall:
    case Debugger::Internal::BreakpointOnQmlSignalEmit:
    case Debugger::Internal::BreakpointAtJavaScriptThrow:
        break;
    case Debugger::Internal::WatchpointAtExpression:
        return !expression.isEmpty();
    case Debugger::Internal::UnknownType:
        return false;
    }
    return true;
}

bool BreakpointParameters::equals(const BreakpointParameters &rhs) const
{
    return !differencesTo(rhs);
}

bool BreakpointParameters::conditionsMatch(const QByteArray &other) const
{
    // Some versions of gdb "beautify" the passed condition.
    QByteArray s1 = condition;
    s1.replace(' ', "");
    QByteArray s2 = other;
    s2.replace(' ', "");
    return s1 == s2;
}

void BreakpointParameters::updateLocation(const QByteArray &location)
{
    if (location.size()) {
        int pos = location.indexOf(':');
        lineNumber = location.mid(pos + 1).toInt();
        QString file = QString::fromUtf8(location.left(pos));
        if (file.startsWith(QLatin1Char('"')) && file.endsWith(QLatin1Char('"')))
            file = file.mid(1, file.size() - 2);
        QFileInfo fi(file);
        if (fi.isReadable())
            fileName = fi.absoluteFilePath();
    }
}

bool BreakpointParameters::isCppBreakpoint() const
{
    // Qml specific breakpoint types.
    if (type == BreakpointAtJavaScriptThrow
            || type == BreakpointOnQmlSignalEmit)
        return false;

    // Qml is currently only file.
    if (type == BreakpointByFileAndLine)
        return !fileName.endsWith(QLatin1String(".qml"), Qt::CaseInsensitive)
                && !fileName.endsWith(QLatin1String(".js"), Qt::CaseInsensitive);

    return true;
}

QString BreakpointParameters::toString() const
{
    QString result;
    QTextStream ts(&result);
    ts << "Type: " << type;
    switch (type) {
    case BreakpointByFileAndLine:
        ts << " FileName: " << fileName << ':' << lineNumber
           << " PathUsage: " << pathUsage;
        break;
    case BreakpointByFunction:
    case BreakpointOnQmlSignalEmit:
        ts << " FunctionName: " << functionName;
        break;
    case BreakpointByAddress:
    case WatchpointAtAddress:
        ts << " Address: " << address;
        break;
    case WatchpointAtExpression:
        ts << " Expression: " << expression;
        break;
    case BreakpointAtThrow:
    case BreakpointAtCatch:
    case BreakpointAtMain:
    case BreakpointAtFork:
    case BreakpointAtExec:
    //case BreakpointAtVFork:
    case BreakpointAtSysCall:
    case BreakpointAtJavaScriptThrow:
    case UnknownType:
        break;
    }
    ts << (enabled ? " [enabled]" : " [disabled]");
    if (!condition.isEmpty())
        ts << " Condition: " << condition;
    if (ignoreCount)
        ts << " IgnoreCount: " << ignoreCount;
    if (tracepoint)
        ts << " [tracepoint]";
    if (!module.isEmpty())
        ts << " Module: " << module;
    if (!command.isEmpty())
        ts << " Command: " << command;
    if (!message.isEmpty())
        ts << " Message: " << message;
    return result;
}

//////////////////////////////////////////////////////////////////
//
// BreakpointResponse
//
//////////////////////////////////////////////////////////////////

/*!
    \class Debugger::Internal::BreakpointResponse

    This is what debuggers produce in response to the attempt to
    insert a breakpoint. The data might differ from the requested bits.
*/

BreakpointResponse::BreakpointResponse()
{
    pending = true;
    hitCount = 0;
    multiple = false;
    correctedLineNumber = 0;
}

QString BreakpointResponse::toString() const
{
    QString result = BreakpointParameters::toString();
    QTextStream ts(&result);
    ts << " Number: " << id.toString();
    if (pending)
        ts << " [pending]";
    if (!functionName.isEmpty())
        ts << " Function: " << functionName;
    if (multiple)
        ts << " Multiple: " << multiple;
    if (correctedLineNumber)
        ts << " CorrectedLineNumber: " << correctedLineNumber;
    ts << " Hit: " << hitCount << " times";
    ts << ' ';
    return result + BreakpointParameters::toString();
}

void BreakpointResponse::fromParameters(const BreakpointParameters &p)
{
    BreakpointParameters::operator=(p);
    id = BreakpointResponseId();
    multiple = false;
    correctedLineNumber = 0;
    hitCount = 0;
}

} // namespace Internal
} // namespace Debugger

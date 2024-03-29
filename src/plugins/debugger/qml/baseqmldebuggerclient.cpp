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

#include "baseqmldebuggerclient.h"
#include "breakpoint.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/qtcassert.h>

namespace Debugger {
namespace Internal {

class BaseQmlDebuggerClientPrivate
{
public:
    QList<QByteArray> sendBuffer;
};

BaseQmlDebuggerClient::BaseQmlDebuggerClient(QmlDebug::QmlDebugConnection* client, QLatin1String clientName)
    : QmlDebugClient(clientName, client),
      d(new BaseQmlDebuggerClientPrivate())
{
}

BaseQmlDebuggerClient::~BaseQmlDebuggerClient()
{
    delete d;
}

bool BaseQmlDebuggerClient::acceptsBreakpoint(const BreakpointModelId &/*id*/)
{
    return false;
}

void BaseQmlDebuggerClient::statusChanged(Status status)
{
    emit newStatus(status);
}

void BaseQmlDebuggerClient::sendMessage(const QByteArray &msg)
{
    if (status() == Enabled) {
        QmlDebugClient::sendMessage(msg);
    } else {
        d->sendBuffer.append(msg);
    }
}

void BaseQmlDebuggerClient::flushSendBuffer()
{
    QTC_ASSERT(status() == QmlDebugClient::Enabled, return);
    foreach (const QByteArray &msg, d->sendBuffer)
       QmlDebugClient::sendMessage(msg);
    d->sendBuffer.clear();
}

} // Internal
} // Debugger

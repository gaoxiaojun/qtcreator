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

#ifndef BASEQMLDEBUGGERCLIENT_H
#define BASEQMLDEBUGGERCLIENT_H

#include "debuggerengine.h"
#include <qmldebug/qmldebugclient.h>

namespace Debugger {
namespace Internal {

class WatchData;
class BreakHandler;
class BreakpointModelId;
class QmlEngine;
class BaseQmlDebuggerClientPrivate;

class BaseQmlDebuggerClient : public QmlDebug::QmlDebugClient
{
    Q_OBJECT

public:
    BaseQmlDebuggerClient(QmlDebug::QmlDebugConnection* client, QLatin1String clientName);
    virtual ~BaseQmlDebuggerClient();

    virtual void startSession() = 0;
    virtual void endSession() = 0;

    virtual void executeStep() = 0;
    virtual void executeStepOut() = 0;
    virtual void executeNext() = 0;
    virtual void executeStepI() = 0;

    virtual void executeRunToLine(const ContextData &data) = 0;

    virtual void continueInferior() = 0;
    virtual void interruptInferior() = 0;

    virtual void activateFrame(int index) = 0;

    virtual bool acceptsBreakpoint(const BreakpointModelId &id);
    virtual void insertBreakpoint(const BreakpointModelId &id, int adjustedLine,
                                  int adjustedColumn = -1) = 0;
    virtual void removeBreakpoint(const BreakpointModelId &id) = 0;
    virtual void changeBreakpoint(const BreakpointModelId &id) = 0;
    virtual void synchronizeBreakpoints() = 0;

    virtual void assignValueInDebugger(const WatchData *data,
                                       const QString &expression,
                                       const QVariant &valueV) = 0;

    virtual void updateWatchData(const WatchData &data) = 0;
    virtual void executeDebuggerCommand(const QString &command) = 0;

    virtual void synchronizeWatchers(const QStringList &watchers) = 0;

    virtual void expandObject(const QByteArray &iname, quint64 objectId) = 0;

    virtual void setEngine(QmlEngine *engine) = 0;

    virtual void getSourceFiles() {}

    void flushSendBuffer();

signals:
    void newStatus(QmlDebugClient::Status status);

protected:
    virtual void statusChanged(Status status);
    void sendMessage(const QByteArray &msg);

private:
    BaseQmlDebuggerClientPrivate *d;
    friend class BaseQmlDebuggerClientPrivate;
};

} // Internal
} // Debugger

#endif // BASEQMLDEBUGGERCLIENT_H

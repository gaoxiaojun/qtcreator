/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DEBUGGER_QMLENGINE_H
#define DEBUGGER_QMLENGINE_H

#include "debuggerengine.h"
#include <qmldebug/qdebugmessageclient.h>
#include <utils/outputformat.h>
#include <qmljs/qmljsdocument.h>

#include <QAbstractSocket>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace Core {
class IEditor;
}

namespace Debugger {

class QmlAdapter;

namespace Internal {

class QtMessageLogItem;
class QmlEnginePrivate;

class DEBUGGER_EXPORT QmlEngine : public DebuggerEngine
{
    Q_OBJECT

public:
    enum LogDirection {
        LogSend,
        LogReceive
    };

    QmlEngine(const DebuggerStartParameters &startParameters,
        DebuggerEngine *masterEngine);
    ~QmlEngine();

    void handleRemoteSetupDone(int gdbServerPort, int qmlPort);
    void handleRemoteSetupFailed(const QString &message);

    bool canDisplayTooltip() const;

    void showMessage(const QString &msg, int channel = LogDebug,
                     int timeout = -1) const;
    void gotoLocation(const Internal::Location &location);

    void filterApplicationMessage(const QString &msg, int channel);
    void inferiorSpontaneousStop();

    void logMessage(const QString &service, LogDirection direction, const QString &str);

    void setSourceFiles(const QStringList &fileNames);
    void updateScriptSource(const QString &fileName, int lineOffset, int columnOffset, const QString &source);

    QmlAdapter *adapter() const;

    void insertBreakpoint(BreakpointModelId id);

public slots:
    void disconnected();
    void documentUpdated(QmlJS::Document::Ptr doc);
    void expressionEvaluated(quint32 queryId, const QVariant &result);

private slots:
    void errorMessageBoxFinished(int result);
    void updateCurrentContext();
    void appendDebugOutput(QtMsgType type, const QString &message,
                           const QmlDebug::QDebugContextInfo &info);

private:
    // DebuggerEngine implementation.
    bool isSynchronous() const { return false; }
    void executeStep();
    void executeStepOut();
    void executeNext();
    void executeStepI();
    void executeNextI();

    void setupEngine();
    void setupInferior();
    void runEngine();
    void shutdownInferior();
    void shutdownEngine();

    bool setToolTipExpression(const QPoint &mousePos,
        TextEditor::ITextEditor *editor, const DebuggerToolTipContext &);

    void continueInferior();
    void interruptInferior();

    void executeRunToLine(const ContextData &data);
    void executeRunToFunction(const QString &functionName);
    void executeJumpToLine(const ContextData &data);

    void activateFrame(int index);
    void selectThread(int index);

    void attemptBreakpointSynchronization();
    void removeBreakpoint(BreakpointModelId id);
    void changeBreakpoint(BreakpointModelId id);
    bool acceptsBreakpoint(BreakpointModelId id) const;

    void assignValueInDebugger(const WatchData *data,
        const QString &expr, const QVariant &value);


    void loadSymbols(const QString &moduleName);
    void loadAllSymbols();
    void requestModuleSymbols(const QString &moduleName);
    void reloadModules();
    void reloadRegisters() {}
    void reloadSourceFiles();
    void reloadFullStack() {}

    bool supportsThreads() const { return false; }
    void updateWatchData(const WatchData &data,
        const WatchUpdateFlags &flags);
    void executeDebuggerCommand(const QString &command, DebuggerLanguages languages);
    bool evaluateScriptExpression(const QString &expression);

    bool hasCapability(unsigned) const;

signals:
    void tooltipRequested(const QPoint &mousePos,
        TextEditor::ITextEditor *editor, int cursorPos);

private slots:
    void tryToConnect(quint16 port = 0);
    void beginConnection(quint16 port = 0);
    void connectionEstablished();
    void connectionStartupFailed();
    void appStartupFailed(const QString &errorMessage);
    void connectionError(QAbstractSocket::SocketError error);
    void serviceConnectionError(const QString &service);
    void appendMessage(const QString &msg, Utils::OutputFormat);

    void synchronizeWatchers();

private:
    void closeConnection();
    void startApplicationLauncher();
    void stopApplicationLauncher();

    bool isShadowBuildProject() const;
    QString fromShadowBuildFilename(const QString &filename) const;
    QString mangleFilenamePaths(const QString &filename,
        const QString &oldBasePath, const QString &newBasePath) const;
    QString qmlImportPath() const;

    void updateEditor(Core::IEditor *editor, const QTextDocument *document);
    bool canEvaluateScript(const QString &script);
    QtMessageLogItem *constructLogItemTree(QtMessageLogItem *parent,
                                           const QVariant &result,
                                           const QString &key = QString());
    bool adjustBreakpointLineAndColumn(const QString &filePath, quint32 *line,
                                       quint32 *column, bool *valid);

private:
    friend class QmlCppEngine;
    QmlEnginePrivate *d;
};

} // namespace Internal
} // namespace Debugger

#endif // DEBUGGER_QMLENGINE_H

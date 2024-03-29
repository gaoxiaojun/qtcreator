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

#ifndef DEBUGGER_GDBENGINE_H
#define DEBUGGER_GDBENGINE_H

#include "debuggerengine.h"

#include "stackframe.h"
#include "watchutils.h"

#include <QByteArray>
#include <QProcess>
#include <QHash>
#include <QMap>
#include <QMultiMap>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QTextCodec>
#include <QTime>
#include <QTimer>
#include <QVariant>


namespace Debugger {
namespace Internal {

class AbstractGdbAdapter;
class AbstractGdbProcess;
class DebugInfoTask;
class DebugInfoTaskHandler;
class GdbResponse;
class GdbMi;
class GdbToolTipContext;

class WatchData;
class DisassemblerAgentCookie;
class DisassemblerLines;

class AttachGdbAdapter;
class CoreGdbAdapter;
class LocalPlainGdbAdapter;
class RemoteGdbServerAdapter;

enum DebuggingHelperState
{
    DebuggingHelperUninitialized,
    DebuggingHelperLoadTried,
    DebuggingHelperAvailable,
    DebuggingHelperUnavailable
};

class UpdateParameters
{
public:
    UpdateParameters() { tryPartial = tooltipOnly = false; }

    bool tryPartial;
    bool tooltipOnly;
    QByteArray varList;
};

/* This is only used with Mac gdb since 2.2
 *
 * "Custom dumper" is a library compiled against the current
 * Qt containing functions to evaluate values of Qt classes
 * (such as QString, taking pointers to their addresses).
 * The library must be loaded into the debuggee.
 * It provides a function that takes input from an input buffer
 * and some parameters and writes output into an output buffer.
 * Parameter 1 is the protocol:
 * 1) Query. Fills output buffer with known types, Qt version and namespace.
 *    This information is parsed and stored by this class (special type
 *    enumeration).
 * 2) Evaluate symbol, taking address and some additional parameters
 *    depending on type. */

class DumperHelper
{
public:
    enum Type {
        UnknownType,
        SupportedType, // A type that requires no special handling by the dumper
        // Below types require special handling
        QAbstractItemType,
        QObjectType, QWidgetType, QObjectSlotType, QObjectSignalType,
        QVectorType, QMapType, QMultiMapType, QMapNodeType, QStackType,
        StdVectorType, StdDequeType, StdSetType, StdMapType, StdStackType,
        StdStringType
    };

    // Type/Parameter struct required for building a value query
    struct TypeData {
        TypeData();
        void clear();

        Type type;
        bool isTemplate;
        QByteArray tmplate;
        QByteArray inner;
    };

    DumperHelper();
    void clear();

    double dumperVersion() const { return m_dumperVersion; }

    int typeCount() const;
    // Look up a simple, non-template  type
    Type simpleType(const QByteArray &simpleType) const;
    // Look up a (potentially) template type and fill parameter struct
    TypeData typeData(const QByteArray &typeName) const;
    Type type(const QByteArray &typeName) const;

    int qtVersion() const;
    QByteArray qtVersionString() const;
    QByteArray qtNamespace() const;
    void setQtNamespace(const QByteArray &ba)
        { if (!ba.isEmpty()) m_qtNamespace = ba; }

    // Complete parse of "query" (protocol 1) response from debuggee buffer.
    // 'data' excludes the leading indicator character.
    bool parseQuery(const GdbMi &data);
    // Sizes can be added as the debugger determines them
    void addSize(const QByteArray &type, int size);

    // Determine the parameters required for an "evaluate" (protocol 2) call
    void evaluationParameters(const WatchData &data,
                              const TypeData &td,
                              QByteArray *inBuffer,
                              QList<QByteArray> *extraParameters) const;

    QString toString(bool debug = false) const;

    static QString msgDumperOutdated(double requiredVersion, double currentVersion);
    static QString msgPtraceError(DebuggerStartMode sm);

private:
    typedef QMap<QByteArray, Type> NameTypeMap;
    typedef QMap<QByteArray, int> SizeCache;

    // Look up a simple (namespace) type
    QByteArray evaluationSizeofTypeExpression(const QByteArray &typeName) const;

    NameTypeMap m_nameTypeMap;
    SizeCache m_sizeCache;

    // The initial dumper query function returns sizes of some special
    // types to aid CDB since it cannot determine the size of classes.
    // They are not complete (std::allocator<X>).
    enum SpecialSizeType { IntSize, PointerSize, StdAllocatorSize,
                           QSharedPointerSize, QSharedDataPointerSize,
                           QWeakPointerSize, QPointerSize,
                           QListSize, QLinkedListSize, QVectorSize, QQueueSize,
                           SpecialSizeCount };

    // Resolve name to enumeration or SpecialSizeCount (invalid)
    SpecialSizeType specialSizeType(const QByteArray &type) const;

    int m_specialSizes[SpecialSizeCount];

    typedef QMap<QByteArray, QByteArray> ExpressionCache;
    ExpressionCache m_expressionCache;
    int m_qtVersion;
    double m_dumperVersion;
    QByteArray m_qtNamespace;

    void setQClassPrefixes(const QByteArray &qNamespace);

    QByteArray m_qPointerPrefix;
    QByteArray m_qSharedPointerPrefix;
    QByteArray m_qSharedDataPointerPrefix;
    QByteArray m_qWeakPointerPrefix;
    QByteArray m_qListPrefix;
    QByteArray m_qLinkedListPrefix;
    QByteArray m_qVectorPrefix;
    QByteArray m_qQueuePrefix;
};

class GdbEngine : public Debugger::DebuggerEngine
{
    Q_OBJECT

public:
    GdbEngine(const DebuggerStartParameters &startParameters,
        DebuggerEngine *masterEngine);
    ~GdbEngine();
    AbstractGdbAdapter *gdbAdapter() const { return m_gdbAdapter; }

private:
    friend class AbstractGdbAdapter;
    friend class AbstractPlainGdbAdapter;
    friend class AttachGdbAdapter;
    friend class CoreGdbAdapter;
    friend class LocalPlainGdbAdapter;
    friend class TermGdbAdapter;
    friend class RemoteGdbServerAdapter;
    friend class RemotePlainGdbAdapter;
    friend class CodaGdbAdapter;

private: ////////// General Interface //////////

    virtual void setupEngine();
    virtual void setupInferior();
    virtual void runEngine();

    virtual bool hasCapability(unsigned) const;
    virtual void detachDebugger();
    virtual void shutdownEngine();
    virtual void shutdownInferior();
    virtual void notifyInferiorSetupFailed();
    virtual void abortDebugger();

    virtual bool acceptsDebuggerCommands() const;
    virtual void executeDebuggerCommand(const QString &command, DebuggerLanguages languages);
    virtual QByteArray qtNamespace() const { return m_dumperHelper.qtNamespace(); }
    virtual void setQtNamespace(const QByteArray &ns)
        { return m_dumperHelper.setQtNamespace(ns); }

private: ////////// General State //////////

    DebuggerStartMode startMode() const;
    Q_SLOT void reloadLocals();

    bool m_registerNamesListed;

private: ////////// Gdb Process Management //////////

    AbstractGdbAdapter *createAdapter();
    bool startGdb(const QStringList &args = QStringList(),
                  const QString &settingsIdHint = QString());
    void handleInferiorShutdown(const GdbResponse &response);
    void handleGdbExit(const GdbResponse &response);
    void handleRemoteSetupDone(int gdbServerPort, int qmlPort);
    void handleRemoteSetupFailed(const QString &message);
    void handleNamespaceExtraction(const GdbResponse &response);

    void handleAdapterStarted();
    void loadInitScript();
    void loadPythonDumpers();
    void pythonDumpersFailed();

    // Something went wrong with the adapter *before* adapterStarted() was emitted.
    // Make sure to clean up everything before emitting this signal.
    void handleAdapterStartFailed(const QString &msg,
        const QString &settingsIdHint = QString());

    // This triggers the initial breakpoint synchronization and causes
    // finishInferiorSetup() being called once done.
    void handleInferiorPrepared();
    // This notifies the base of a successful inferior setup.
    void finishInferiorSetup();

    // The adapter is still running just fine, but it failed to acquire a debuggee.
    void notifyInferiorSetupFailed(const QString &msg);

    void notifyAdapterShutdownOk();
    void notifyAdapterShutdownFailed();

    // Something went wrong with the adapter *after* adapterStarted() was emitted.
    // Make sure to clean up everything before emitting this signal.
    void handleAdapterCrashed(const QString &msg);

private slots:
    void handleGdbFinished(int, QProcess::ExitStatus status);
    void handleGdbError(QProcess::ProcessError error);
    void readGdbStandardOutput();
    void readGdbStandardError();
    void readDebugeeOutput(const QByteArray &data);

private:
    QTextCodec *m_outputCodec;
    QTextCodec::ConverterState m_outputCodecState;

    QByteArray m_inbuffer;
    bool m_busy;

    AbstractGdbAdapter *m_gdbAdapter;

    // Name of the convenience variable containing the last
    // known function return value.
    QByteArray m_resultVarName;

private: ////////// Gdb Command Management //////////

    public: // Otherwise the Qt flag macros are unhappy.
    enum GdbCommandFlag {
        NoFlags = 0,
        // The command needs a stopped inferior.
        NeedsStop = 1,
        // No need to wait for the reply before continuing inferior.
        Discardable = 2,
        // Trigger watch model rebuild when no such commands are pending anymore.
        RebuildWatchModel = 4,
        WatchUpdate = Discardable | RebuildWatchModel,
        // We can live without receiving an answer.
        NonCriticalResponse = 8,
        // Callback expects GdbResultRunning instead of GdbResultDone.
        RunRequest = 16,
        // Callback expects GdbResultExit instead of GdbResultDone.
        ExitRequest = 32,
        // Auto-set inferior shutdown related states.
        LosesChild = 64,
        // Trigger breakpoint model rebuild when no such commands are pending anymore.
        RebuildBreakpointModel = 128,
        // This command needs to be send immediately.
        Immediate = 256,
        // This is a command that needs to be wrapped into -interpreter-exec console
        ConsoleCommand = 512
    };
    Q_DECLARE_FLAGS(GdbCommandFlags, GdbCommandFlag)
    private:

    typedef void (GdbEngine::*GdbCommandCallback)
        (const GdbResponse &response);
    typedef void (AbstractGdbAdapter::*AdapterCallback)
        (const GdbResponse &response);

    struct GdbCommand
    {
        GdbCommand()
            : flags(0), callback(0), adapterCallback(0), callbackName(0)
        {}

        int flags;
        GdbCommandCallback callback;
        AdapterCallback adapterCallback;
        const char *callbackName;
        QByteArray command;
        QVariant cookie;
        QTime postTime;
    };

    // Type and cookie are sender-internal data, opaque for the "event
    // queue". resultNeeded == true increments m_pendingResults on
    // send and decrements on receipt, effectively preventing
    // watch model updates before everything is finished.
    void flushCommand(const GdbCommand &cmd);
    void postCommand(const QByteArray &command,
                     GdbCommandFlags flags,
                     GdbCommandCallback callback = 0,
                     const char *callbackName = 0,
                     const QVariant &cookie = QVariant());
    void postCommand(const QByteArray &command,
                     GdbCommandCallback callback = 0,
                     const char *callbackName = 0,
                     const QVariant &cookie = QVariant());
    void postCommand(const QByteArray &command,
                     AdapterCallback callback,
                     const char *callbackName,
                     const QVariant &cookie = QVariant());
    void postCommand(const QByteArray &command,
                     GdbCommandFlags flags,
                     AdapterCallback callback,
                     const char *callbackName,
                     const QVariant &cookie = QVariant());
    void postCommandHelper(const GdbCommand &cmd);
    void flushQueuedCommands();
    Q_SLOT void commandTimeout();
    void setTokenBarrier();

    // Sets up an "unexpected result" for the following commeand.
    void scheduleTestResponse(int testCase, const QByteArray &response);

    QHash<int, GdbCommand> m_cookieForToken;
    int commandTimeoutTime() const;
    QTimer m_commandTimer;

    QByteArray m_pendingConsoleStreamOutput;
    QByteArray m_pendingLogStreamOutput;

    // This contains the first token number for the current round
    // of evaluation. Responses with older tokens are considers
    // out of date and discarded.
    int m_oldestAcceptableToken;
    int m_nonDiscardableCount;

    int m_pendingWatchRequests; // Watch updating commands in flight
    int m_pendingBreakpointRequests; // Watch updating commands in flight

    typedef void (GdbEngine::*CommandsDoneCallback)();
    // This function is called after all previous responses have been received.
    CommandsDoneCallback m_commandsDoneCallback;

    QList<GdbCommand> m_commandsToRunOnTemporaryBreak;
    int gdbVersion() const { return m_gdbVersion; }

private: ////////// Gdb Output, State & Capability Handling //////////

    Q_SLOT void handleResponse(const QByteArray &buff);
    void handleStopResponse(const GdbMi &data);
    void handleResultRecord(GdbResponse *response);
    void handleStop1(const GdbResponse &response);
    void handleStop1(const GdbMi &data);
    void handleStop2(const GdbResponse &response);
    void handleStop2(const GdbMi &data);
    Q_SLOT void handleStop2();
    StackFrame parseStackFrame(const GdbMi &mi, int level);
    void resetCommandQueue();

    bool isSynchronous() const { return hasPython(); }
    virtual bool hasPython() const;
    bool supportsThreads() const;

    // Gdb initialization sequence
    void handleShowVersion(const GdbResponse &response);
    void handleListFeatures(const GdbResponse &response);
    void handleHasPython(const GdbResponse &response);

    int m_gdbVersion; // 6.8.0 is 60800
    int m_gdbBuildVersion; // MAC only?
    bool m_isMacGdb;
    bool m_isQnxGdb;
    bool m_hasBreakpointNotifications;
    bool m_hasPython;
    bool m_hasInferiorThreadList;

private: ////////// Inferior Management //////////

    // This should be always the last call in a function.
    bool stateAcceptsBreakpointChanges() const;
    bool acceptsBreakpoint(BreakpointModelId id) const;
    void insertBreakpoint(BreakpointModelId id);
    void removeBreakpoint(BreakpointModelId id);
    void changeBreakpoint(BreakpointModelId id);

    void executeStep();
    void executeStepOut();
    void executeNext();
    void executeStepI();
    void executeNextI();

    void continueInferiorInternal();
    void doNotifyInferiorRunOk();
    void autoContinueInferior();
    void continueInferior();
    void interruptInferior();
    void interruptInferiorTemporarily();

    void executeRunToLine(const ContextData &data);
    void executeRunToFunction(const QString &functionName);
    void executeJumpToLine(const ContextData &data);
    void executeReturn();

    void handleExecuteContinue(const GdbResponse &response);
    void handleExecuteStep(const GdbResponse &response);
    void handleExecuteNext(const GdbResponse &response);
    void handleExecuteReturn(const GdbResponse &response);
    void handleExecuteJumpToLine(const GdbResponse &response);
    void handleExecuteRunToLine(const GdbResponse &response);

    void maybeHandleInferiorPidChanged(const QString &pid);
    void handleInfoProc(const GdbResponse &response);

private: ////////// View & Data Stuff //////////

    void selectThread(int index);
    void activateFrame(int index);

    //
    // Breakpoint specific stuff
    //
    void handleBreakList(const GdbResponse &response);
    void handleBreakList(const GdbMi &table);
    void handleBreakModifications(const GdbMi &bkpts);
    void handleBreakListMultiple(const GdbResponse &response);
    void handleBreakIgnore(const GdbResponse &response);
    void handleBreakDisable(const GdbResponse &response);
    void handleBreakEnable(const GdbResponse &response);
    void handleBreakInsert1(const GdbResponse &response);
    void handleBreakInsert2(const GdbResponse &response);
    void handleTraceInsert2(const GdbResponse &response);
    void handleBreakCondition(const GdbResponse &response);
    void handleBreakThreadSpec(const GdbResponse &response);
    void handleBreakLineNumber(const GdbResponse &response);
    void handleWatchInsert(const GdbResponse &response);
    void handleCatchInsert(const GdbResponse &response);
    void handleInfoLine(const GdbResponse &response);
    void extractDataFromInfoBreak(const QString &output, BreakpointModelId);
    void updateResponse(BreakpointResponse &response, const GdbMi &bkpt);
    QByteArray breakpointLocation(BreakpointModelId id); // For gdb/MI.
    QByteArray breakpointLocation2(BreakpointModelId id); // For gdb/CLI fallback.
    QString breakLocation(const QString &file) const;
    void reloadBreakListInternal();
    void attemptAdjustBreakpointLocation(BreakpointModelId id);

    //
    // Modules specific stuff
    //
    void loadSymbols(const QString &moduleName);
    void loadAllSymbols();
    void loadSymbolsForStack();
    void requestModuleSymbols(const QString &moduleName);
    void reloadModules();
    void examineModules();
    void reloadModulesInternal();
    void handleModulesList(const GdbResponse &response);
    void handleShowModuleSymbols(const GdbResponse &response);

    bool m_modulesListOutdated;

    //
    // Snapshot specific stuff
    //
    virtual void createSnapshot();
    void handleMakeSnapshot(const GdbResponse &response);

    //
    // Register specific stuff
    //
    Q_SLOT void reloadRegisters();
    void setRegisterValue(int nr, const QString &value);
    void handleRegisterListNames(const GdbResponse &response);
    void handleRegisterListValues(const GdbResponse &response);

    //
    // Disassembler specific stuff
    //
    // Chain of fallbacks: PointMixed -> PointPlain -> RangeMixed -> RangePlain.
    // The Mi versions are not used right now.
    void fetchDisassembler(DisassemblerAgent *agent);
    void fetchDisassemblerByCliPointMixed(const DisassemblerAgentCookie &ac);
    void fetchDisassemblerByCliPointPlain(const DisassemblerAgentCookie &ac);
    void fetchDisassemblerByCliRangeMixed(const DisassemblerAgentCookie &ac);
    void fetchDisassemblerByCliRangePlain(const DisassemblerAgentCookie &ac);
    //void fetchDisassemblerByMiPointMixed(const DisassemblerAgentCookie &ac);
    //void fetchDisassemblerByMiPointPlain(const DisassemblerAgentCookie &ac);
    //void fetchDisassemblerByMiRangeMixed(const DisassemblerAgentCookie &ac);
    //void fetchDisassemblerByMiRangePlain(const DisassemblerAgentCookie &ac);
    void handleFetchDisassemblerByCliPointMixed(const GdbResponse &response);
    void handleFetchDisassemblerByCliPointPlain(const GdbResponse &response);
    void handleFetchDisassemblerByCliRangeMixed(const GdbResponse &response);
    void handleFetchDisassemblerByCliRangePlain(const GdbResponse &response);
    //void handleFetchDisassemblerByMiPointMixed(const GdbResponse &response);
    //void handleFetchDisassemblerByMiPointPlain(const GdbResponse &response);
    //void handleFetchDisassemblerByMiRangeMixed(const GdbResponse &response);
    //void handleFetchDisassemblerByMiRangePlain(const GdbResponse &response);
    void handleDisassemblerCheck(const GdbResponse &response);
    void handleBreakOnQFatal(const GdbResponse &response);
    DisassemblerLines parseDisassembler(const GdbResponse &response);
    DisassemblerLines parseCliDisassembler(const QByteArray &response);
    DisassemblerLines parseMiDisassembler(const GdbMi &response);

    bool m_disassembleUsesComma;

    //
    // Source file specific stuff
    //
    void reloadSourceFiles();
    void reloadSourceFilesInternal();
    void handleQuerySources(const GdbResponse &response);

    QString fullName(const QString &fileName);
    QString cleanupFullName(const QString &fileName);

    // awful hack to keep track of used files
    QMap<QString, QString> m_shortToFullName;
    QMap<QString, QString> m_fullToShortName;
    QMultiMap<QString, QString> m_baseNameToFullName;

    void invalidateSourcesList();
    bool m_sourcesListUpdating;
    bool m_breakListOutdated;

    //
    // Stack specific stuff
    //
    void updateAll();
        void updateAllClassic();
        void updateAllPython();
    void handleStackListFrames(const GdbResponse &response);
    void handleStackSelectThread(const GdbResponse &response);
    void handleStackSelectFrame(const GdbResponse &response);
    void handleThreadListIds(const GdbResponse &response);
    void handleThreadInfo(const GdbResponse &response);
    void handleThreadNames(const GdbResponse &response);
    Q_SLOT void reloadStack(bool forceGotoLocation);
    Q_SLOT virtual void reloadFullStack();
    int currentFrame() const;

    QList<GdbMi> m_currentFunctionArgs;

    //
    // Watch specific stuff
    //
    virtual bool setToolTipExpression(const QPoint &mousePos,
        TextEditor::ITextEditor *editor, const DebuggerToolTipContext &);
    virtual void assignValueInDebugger(const WatchData *data,
        const QString &expr, const QVariant &value);

    virtual void fetchMemory(MemoryAgent *agent, QObject *token,
        quint64 addr, quint64 length);
    void handleChangeMemory(const GdbResponse &response);
    virtual void changeMemory(MemoryAgent *agent, QObject *token,
        quint64 addr, const QByteArray &data);
    void handleFetchMemory(const GdbResponse &response);

    virtual void watchPoint(const QPoint &);
    void handleWatchPoint(const GdbResponse &response);

    // FIXME: BaseClass. called to improve situation for a watch item
    void updateSubItemClassic(const WatchData &data);

    void virtual updateWatchData(const WatchData &data, const WatchUpdateFlags &flags);
    Q_SLOT void updateWatchDataHelper(const WatchData &data);
    void rebuildWatchModel();
    bool showToolTip();

    void insertData(const WatchData &data);
    void sendWatchParameters(const QByteArray &params0);
    void createGdbVariableClassic(const WatchData &data);

    void runDebuggingHelperClassic(const WatchData &data, bool dumpChildren);
    void runDirectDebuggingHelperClassic(const WatchData &data, bool dumpChildren);
    bool hasDebuggingHelperForType(const QByteArray &type) const;

    void handleVarListChildrenClassic(const GdbResponse &response);
    void handleVarListChildrenHelperClassic(const GdbMi &child,
        const WatchData &parent, int sortId);
    void handleVarCreate(const GdbResponse &response);
    void handleVarAssign(const GdbResponse &response);
    void handleEvaluateExpressionClassic(const GdbResponse &response);
    void handleQueryDebuggingHelperClassic(const GdbResponse &response);
    void handleDebuggingHelperValue2Classic(const GdbResponse &response);
    void handleDebuggingHelperValue3Classic(const GdbResponse &response);
    void handleDebuggingHelperEditValue(const GdbResponse &response);
    void handleDebuggingHelperSetup(const GdbResponse &response);
    void handleDebuggingHelperVersionCheckClassic(const GdbResponse &response);
    void handleDetach(const GdbResponse &response);

    void handleThreadGroupCreated(const GdbMi &result);
    void handleThreadGroupExited(const GdbMi &result);

    Q_SLOT void createFullBacktrace();
    void handleCreateFullBacktrace(const GdbResponse &response);

    void updateLocals();
        void updateLocalsClassic();
        void updateLocalsPython(const UpdateParameters &parameters);
            void handleStackFramePython(const GdbResponse &response);

    void handleStackListLocalsClassic(const GdbResponse &response);
    void handleStackListLocalsPython(const GdbResponse &response);

    WatchData localVariable(const GdbMi &item,
                            const QStringList &uninitializedVariables,
                            QMap<QByteArray, int> *seen);
    void setLocals(const QList<GdbMi> &locals);
    void handleStackListArgumentsClassic(const GdbResponse &response);

    QSet<QByteArray> m_processedNames;
    struct TypeInfo
    {
        TypeInfo(uint s = 0) : size(s) {}

        uint size;
    };

    QHash<QByteArray, TypeInfo> m_typeInfoCache;

    //
    // Dumper Management
    //
    bool checkDebuggingHelpersClassic();
    void setDebuggingHelperStateClassic(DebuggingHelperState);
    void tryLoadDebuggingHelpersClassic();
    void tryQueryDebuggingHelpersClassic();

    DebuggingHelperState m_debuggingHelperState;
    DumperHelper m_dumperHelper;
    QString m_gdb;

    //
    // Convenience Functions
    //
    QString errorMessage(QProcess::ProcessError error);
    AbstractGdbProcess *gdbProc() const;
    void showExecutionError(const QString &message);

    static QByteArray tooltipIName(const QString &exp);
    QString tooltipExpression() const;
    void clearToolTip();
    QScopedPointer<GdbToolTipContext> m_toolTipContext;


    // For short-circuiting stack and thread list evaluation.
    bool m_stackNeeded;
    int m_currentThreadId;

    //
    // Qml
    //
    BreakpointResponseId m_qmlBreakpointResponseId1;
    BreakpointResponseId m_qmlBreakpointResponseId2;
    bool m_preparedForQmlBreak;
    bool setupQmlStep(bool on);
    void handleSetQmlStepBreakpoint(const GdbResponse &response);
    bool isQmlStepBreakpoint(const BreakpointResponseId &id) const;
    bool isQmlStepBreakpoint1(const BreakpointResponseId &id) const;
    bool isQmlStepBreakpoint2(const BreakpointResponseId &id) const;
    bool isQFatalBreakpoint(const BreakpointResponseId &id) const;
    bool isHiddenBreakpoint(const BreakpointResponseId &id) const;

    // HACK:
    QByteArray m_currentThread;
    QString m_lastWinException;
    QString m_lastMissingDebugInfo;
    BreakpointResponseId m_qFatalBreakpointResponseId;
    bool m_actingOnExpectedStop;

    bool usesExecInterrupt() const;

    QHash<int, QByteArray> m_scheduledTestResponses;
    QSet<int> m_testCases;

    // Debug information
    friend class DebugInfoTaskHandler;
    void requestDebugInformation(const DebugInfoTask &task);
    DebugInfoTaskHandler *m_debugInfoTaskHandler;

    // Indicates whether we had at least one full attempt to load
    // debug information.
    bool attemptQuickStart() const;
    bool m_fullStartDone;
};

} // namespace Internal
} // namespace Debugger

Q_DECLARE_OPERATORS_FOR_FLAGS(Debugger::Internal::GdbEngine::GdbCommandFlags)

#endif // DEBUGGER_GDBENGINE_H

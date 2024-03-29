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

#include "runconfiguration.h"

#include "project.h"
#include "target.h"
#include "toolchain.h"
#include "abi.h"
#include "buildconfiguration.h"
#include "projectexplorerconstants.h"
#include <extensionsystem/pluginmanager.h>

#include <utils/qtcassert.h>
#include <utils/outputformatter.h>
#include <utils/checkablemessagebox.h>

#include <coreplugin/icore.h>

#include <QTimer>
#include <QSettings>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

using namespace ProjectExplorer;

namespace {

const char USE_CPP_DEBUGGER_KEY[] = "RunConfiguration.UseCppDebugger";
const char USE_QML_DEBUGGER_KEY[] = "RunConfiguration.UseQmlDebugger";
const char USE_QML_DEBUGGER_AUTO_KEY[] = "RunConfiguration.UseQmlDebuggerAuto";
const char QML_DEBUG_SERVER_PORT_KEY[] = "RunConfiguration.QmlDebugServerPort";
const char USE_MULTIPROCESS_KEY[] = "RunConfiguration.UseMultiProcess";

// Function objects:

class RunConfigurationFactoryMatcher
{
public:
    RunConfigurationFactoryMatcher(Target * target) : m_target(target)
    { }

    virtual ~RunConfigurationFactoryMatcher() { }

    virtual bool operator()(IRunConfigurationFactory *) const = 0;

    Target *target() const
    {
        return m_target;
    }

private:
    Target *m_target;
};

class CreateMatcher : public RunConfigurationFactoryMatcher
{
public:
    CreateMatcher(Target *target, const Core::Id id) :
        RunConfigurationFactoryMatcher(target),
        m_id(id)
    { }

    bool operator()(IRunConfigurationFactory *factory) const
    {
        return factory->canCreate(target(), m_id);
    }

private:
    const Core::Id m_id;
};

class CloneMatcher : public RunConfigurationFactoryMatcher
{
public:
    CloneMatcher(Target *target, RunConfiguration *source) :
        RunConfigurationFactoryMatcher(target),
        m_source(source)
    { }

    bool operator()(IRunConfigurationFactory *factory) const
    {
        return factory->canClone(target(), m_source);
    }

private:
    RunConfiguration *m_source;
};

class RestoreMatcher : public RunConfigurationFactoryMatcher
{
public:
    RestoreMatcher(Target *target, const QVariantMap &map) :
        RunConfigurationFactoryMatcher(target),
        m_map(map)
    { }

    bool operator()(IRunConfigurationFactory *factory) const
    {
        return factory->canRestore(target(), m_map);
    }

private:
    QVariantMap m_map;
};

// Helper methods:

IRunConfigurationFactory *findRunConfigurationFactory(RunConfigurationFactoryMatcher &matcher)
{
    QList<IRunConfigurationFactory *> factories
        = ExtensionSystem::PluginManager::instance()->
                getObjects<IRunConfigurationFactory>();
    foreach (IRunConfigurationFactory *factory, factories) {
        if (matcher(factory))
            return factory;
    }
    return 0;
}

} // namespace

/*!
    \class ProjectExplorer::ProcessHandle
    \brief  Helper class to describe a process.

    Encapsulates parameters of a running process, local (PID) or remote (to be done,
    address, port, etc).
*/

ProcessHandle::ProcessHandle(quint64 pid) :
    m_pid(pid)
{
}

bool ProcessHandle::isValid() const
{
    return m_pid != 0;
}

void ProcessHandle::setPid(quint64 pid)
{
    m_pid = pid;
}

quint64 ProcessHandle::pid() const
{
    return m_pid;
}

QString ProcessHandle::toString() const
{
    if (m_pid)
        return RunControl::tr("PID %1").arg(m_pid);
    //: Invalid process handle.
    return RunControl::tr("Invalid");
}

bool ProcessHandle::equals(const ProcessHandle &rhs) const
{
    return m_pid == rhs.m_pid;
}

/*!
    \class ProjectExplorer::DebuggerRunConfigurationAspect
*/

DebuggerRunConfigurationAspect::DebuggerRunConfigurationAspect(RunConfiguration *rc) :
    m_runConfiguration(rc),
    m_useCppDebugger(true),
    m_useQmlDebugger(AutoEnableQmlDebugger),
    m_qmlDebugServerPort(Constants::QML_DEFAULT_DEBUG_SERVER_PORT),
    m_useMultiProcess(false),
    m_suppressDisplay(false),
    m_suppressQmlDebuggingOptions(false),
    m_suppressCppDebuggingOptions(false),
    m_suppressQmlDebuggingSpinbox(false)
{}

DebuggerRunConfigurationAspect::DebuggerRunConfigurationAspect(DebuggerRunConfigurationAspect *other) :
    m_runConfiguration(other->m_runConfiguration),
    m_useCppDebugger(other->m_useCppDebugger),
    m_useQmlDebugger(other->m_useQmlDebugger),
    m_qmlDebugServerPort(other->m_qmlDebugServerPort),
    m_useMultiProcess(other->m_useMultiProcess),
    m_suppressDisplay(other->m_suppressDisplay),
    m_suppressQmlDebuggingOptions(other->m_suppressQmlDebuggingOptions),
    m_suppressCppDebuggingOptions(other->m_suppressCppDebuggingOptions),
    m_suppressQmlDebuggingSpinbox(other->m_suppressQmlDebuggingSpinbox)
{}

RunConfiguration *DebuggerRunConfigurationAspect::runConfiguration()
{
    return m_runConfiguration;
}

void DebuggerRunConfigurationAspect::setUseQmlDebugger(bool value)
{
    m_useQmlDebugger = value ? EnableQmlDebugger : DisableQmlDebugger;
    emit debuggersChanged();
}

void DebuggerRunConfigurationAspect::setUseCppDebugger(bool value)
{
    m_useCppDebugger = value;
    emit debuggersChanged();
}

bool DebuggerRunConfigurationAspect::useCppDebugger() const
{
    return m_useCppDebugger;
}

static bool isQtQuickAppProject(Project *project)
{
    const QString filePath = project->projectDirectory()
            + QLatin1String("/qmlapplicationviewer/qmlapplicationviewer.pri");
    return project->files(Project::ExcludeGeneratedFiles).contains(filePath);
}

bool DebuggerRunConfigurationAspect::useQmlDebugger() const
{
    if (m_useQmlDebugger == DebuggerRunConfigurationAspect::AutoEnableQmlDebugger)
        return isQtQuickAppProject(m_runConfiguration->target()->project());
    return m_useQmlDebugger == DebuggerRunConfigurationAspect::EnableQmlDebugger;
}

uint DebuggerRunConfigurationAspect::qmlDebugServerPort() const
{
    return m_qmlDebugServerPort;
}

void DebuggerRunConfigurationAspect::setQmllDebugServerPort(uint port)
{
    m_qmlDebugServerPort = port;
}

bool DebuggerRunConfigurationAspect::useMultiProcess() const
{
    return m_useMultiProcess;
}

void DebuggerRunConfigurationAspect::setUseMultiProcess(bool value)
{
    m_useMultiProcess = value;
}

void DebuggerRunConfigurationAspect::suppressDisplay()
{
    m_suppressDisplay = true;
}

void DebuggerRunConfigurationAspect::suppressQmlDebuggingOptions()
{
    m_suppressQmlDebuggingOptions = true;
}

void DebuggerRunConfigurationAspect::suppressCppDebuggingOptions()
{
    m_suppressCppDebuggingOptions = true;
}

void DebuggerRunConfigurationAspect::suppressQmlDebuggingSpinbox()
{
    m_suppressQmlDebuggingSpinbox = true;
}

bool DebuggerRunConfigurationAspect::isDisplaySuppressed() const
{
    return m_suppressDisplay;
}

bool DebuggerRunConfigurationAspect::areQmlDebuggingOptionsSuppressed() const
{
    return m_suppressQmlDebuggingOptions;
}

bool DebuggerRunConfigurationAspect::areCppDebuggingOptionsSuppressed() const
{
    return m_suppressCppDebuggingOptions;
}

bool DebuggerRunConfigurationAspect::isQmlDebuggingSpinboxSuppressed() const
{
    return m_suppressQmlDebuggingSpinbox;
}

QString DebuggerRunConfigurationAspect::displayName() const
{
    return tr("Debugger settings");
}

QVariantMap DebuggerRunConfigurationAspect::toMap() const
{
    QVariantMap map;
    map.insert(QLatin1String(USE_CPP_DEBUGGER_KEY), m_useCppDebugger);
    map.insert(QLatin1String(USE_QML_DEBUGGER_KEY), m_useQmlDebugger == EnableQmlDebugger);
    map.insert(QLatin1String(USE_QML_DEBUGGER_AUTO_KEY), m_useQmlDebugger == AutoEnableQmlDebugger);
    map.insert(QLatin1String(QML_DEBUG_SERVER_PORT_KEY), m_qmlDebugServerPort);
    map.insert(QLatin1String(USE_MULTIPROCESS_KEY), m_useMultiProcess);
    return map;
}

void DebuggerRunConfigurationAspect::fromMap(const QVariantMap &map)
{
    m_useCppDebugger = map.value(QLatin1String(USE_CPP_DEBUGGER_KEY), true).toBool();
    if (map.value(QLatin1String(USE_QML_DEBUGGER_AUTO_KEY), false).toBool()) {
        m_useQmlDebugger = AutoEnableQmlDebugger;
    } else {
        bool useQml = map.value(QLatin1String(USE_QML_DEBUGGER_KEY), false).toBool();
        m_useQmlDebugger = useQml ? EnableQmlDebugger : DisableQmlDebugger;
    }
    m_useMultiProcess = map.value(QLatin1String(USE_MULTIPROCESS_KEY), false).toBool();
}


/*!
    \class ProjectExplorer::RunConfiguration
    \brief  Base class for a run configuration. A run configuration specifies how a
    target should be run, while the runner (see below) does the actual running.

    Note that all RunControls and the target hold a shared pointer to the RunConfiguration.
    That is the lifetime of the RunConfiguration might exceed the life of the target.
    The user might still have a RunControl running (or output tab of that RunControl open)
    and yet unloaded the target.

    Also, a RunConfiguration might be already removed from the list of RunConfigurations
    for a target, but still be runnable via the output tab.
*/

RunConfiguration::RunConfiguration(Target *target, const Core::Id id) :
    ProjectConfiguration(target, id),
    m_debuggerAspect(new DebuggerRunConfigurationAspect(this))
{
    Q_ASSERT(target);
    addExtraAspects();
}

RunConfiguration::RunConfiguration(Target *target, RunConfiguration *source) :
    ProjectConfiguration(target, source),
    m_debuggerAspect(new DebuggerRunConfigurationAspect(source->debuggerAspect()))
{
    Q_ASSERT(target);
    addExtraAspects();
}

RunConfiguration::~RunConfiguration()
{
    delete m_debuggerAspect;
    qDeleteAll(m_aspects);
}

void RunConfiguration::addExtraAspects()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    foreach (IRunControlFactory *factory, pm->getObjects<IRunControlFactory>())
        if (IRunConfigurationAspect *aspect = factory->createRunConfigurationAspect())
            m_aspects.append(aspect);
}

/*!
    \brief Used to find out whether a runconfiguration is enabled
*/

bool RunConfiguration::isEnabled() const
{
    return true;
}

QString RunConfiguration::disabledReason() const
{
    return QString();
}

/*!
    \fn virtual QWidget *ProjectExplorer::RunConfiguration::createConfigurationWidget()

    \brief Returns the widget used to configure this run configuration. Ownership is transferred to the caller
*/

BuildConfiguration *RunConfiguration::activeBuildConfiguration() const
{
    if (!target())
        return 0;
    return target()->activeBuildConfiguration();
}

Target *RunConfiguration::target() const
{
    return static_cast<Target *>(parent());
}

QVariantMap RunConfiguration::toMap() const
{
    QVariantMap map = ProjectConfiguration::toMap();

    map.unite(m_debuggerAspect->toMap());

    foreach (IRunConfigurationAspect *aspect, m_aspects)
        map.unite(aspect->toMap());

    return map;
}

ProjectExplorer::Abi RunConfiguration::abi() const
{
    BuildConfiguration *bc = target()->activeBuildConfiguration();
    if (!bc)
        return Abi::hostAbi();
    ToolChain *tc = bc->toolChain();
    if (!tc)
        return Abi::hostAbi();
    return tc->targetAbi();
}

bool RunConfiguration::fromMap(const QVariantMap &map)
{
    m_debuggerAspect->fromMap(map);

    foreach (IRunConfigurationAspect *aspect, m_aspects)
        aspect->fromMap(map);

    return ProjectConfiguration::fromMap(map);
}

/*!
    \class ProjectExplorer::IRunConfigurationAspect

    \brief Extra configuration aspect.

    Aspects are a mechanism to add RunControl-specific options to a RunConfiguration without
    subclassing the RunConfiguration for every addition, preventing a combinatorical explosion
    of subclasses or the need to add all options to the base class.
*/

/*!
    \brief Return extra aspects.

    \sa ProjectExplorer::IRunConfigurationAspect
*/

QList<IRunConfigurationAspect *> RunConfiguration::extraAspects() const
{
    return m_aspects;
}

Utils::OutputFormatter *RunConfiguration::createOutputFormatter() const
{
    return new Utils::OutputFormatter();
}


/*!
    \class ProjectExplorer::IRunConfigurationFactory

    \brief Restores RunConfigurations from settings.

    The run configuration factory is used for restoring run configurations from
    settings. And used to create new runconfigurations in the "Run Settings" Dialog.
    For the first case, bool canRestore(Target *parent, const QString &id) and
    RunConfiguration* create(Target *parent, const QString &id) are used.
    For the second type, the functions QStringList availableCreationIds(Target *parent) and
    QString displayNameForType(const QString&) are used to generate a list of creatable
    RunConfigurations, and create(..) is used to create it.
*/

/*!
    \fn QStringList ProjectExplorer::IRunConfigurationFactory::availableCreationIds(Target *parent) const

    \brief Used to show the list of possible additons to a target, returns a list of types.
*/

/*!
    \fn QString ProjectExplorer::IRunConfigurationFactory::displayNameForId(const QString &id) const
    \brief Used to translate the types to names to display to the user.
*/

IRunConfigurationFactory::IRunConfigurationFactory(QObject *parent) :
    QObject(parent)
{
}

IRunConfigurationFactory::~IRunConfigurationFactory()
{
}

IRunConfigurationFactory *IRunConfigurationFactory::createFactory(Target *parent, const Core::Id id)
{
    CreateMatcher matcher(parent, id);
    return findRunConfigurationFactory(matcher);
}

IRunConfigurationFactory *IRunConfigurationFactory::cloneFactory(Target *parent, RunConfiguration *source)
{
    CloneMatcher matcher(parent, source);
    return findRunConfigurationFactory(matcher);
}

IRunConfigurationFactory *IRunConfigurationFactory::restoreFactory(Target *parent, const QVariantMap &map)
{
    RestoreMatcher matcher(parent, map);
    return findRunConfigurationFactory(matcher);
}

/*!
    \class ProjectExplorer::IRunControlFactory

    \brief Creates RunControl objects matching a RunConfiguration
*/

/*!
    \fn IRunConfigurationAspect *ProjectExplorer::IRunControlFactory::createRunConfigurationAspect()
    \brief Return an IRunConfigurationAspect to carry options for RunControls this factory can create.

    If no extra options are required it is allowed to return null like the default implementation does.
    This is intended to be called from the RunConfiguration constructor, so passing a RunConfiguration
    pointer makes no sense because that object is under construction at the time.
*/

/*!
    \fn RunConfigWidget *ProjectExplorer::IRunControlFactory::createConfigurationWidget(RunConfiguration *runConfiguration)

    \brief Return a widget used to configure this runner. Ownership is transferred to the caller.

    Return 0 if @p runConfiguration is not suitable for RunControls from this factory, or no user-accessible
    configuration is required.
*/

IRunControlFactory::IRunControlFactory(QObject *parent)
    : QObject(parent)
{
}

IRunControlFactory::~IRunControlFactory()
{
}

IRunConfigurationAspect *IRunControlFactory::createRunConfigurationAspect()
{
    return 0;
}

/*!
    \class ProjectExplorer::RunControl
    \brief Each instance of this class represents one item that is run.
*/

/*!
    \fn bool ProjectExplorer::RunControl::promptToStop(bool *optionalPrompt = 0) const

    \brief Prompt to stop. If 'optionalPrompt' is passed, a "Do not ask again"-
    checkbox will show and the result will be returned in '*optionalPrompt'.
*/

/*!
    \fn QIcon ProjectExplorer::RunControl::icon() const
    \brief Eeturns the icon to be shown in the Outputwindow.

    TODO the icon differs currently only per "mode", so this is more flexible then it needs to be.
*/

RunControl::RunControl(RunConfiguration *runConfiguration, RunMode mode)
    : m_runMode(mode), m_runConfiguration(runConfiguration), m_outputFormatter(0)
{
    if (runConfiguration) {
        m_displayName  = runConfiguration->displayName();
        m_outputFormatter = runConfiguration->createOutputFormatter();
    }
    // We need to ensure that there's always a OutputFormatter
    if (!m_outputFormatter)
        m_outputFormatter = new Utils::OutputFormatter();
}

RunControl::~RunControl()
{
    delete m_outputFormatter;
}

Utils::OutputFormatter *RunControl::outputFormatter()
{
    return m_outputFormatter;
}

RunMode RunControl::runMode() const
{
    return m_runMode;
}

QString RunControl::displayName() const
{
    return m_displayName;
}

Abi RunControl::abi() const
{
    if (const RunConfiguration *rc = m_runConfiguration.data())
        return rc->abi();
    return Abi();
}

ProcessHandle RunControl::applicationProcessHandle() const
{
    return m_applicationProcessHandle;
}

void RunControl::setApplicationProcessHandle(const ProcessHandle &handle)
{
    if (m_applicationProcessHandle != handle) {
        m_applicationProcessHandle = handle;
        emit applicationProcessHandleChanged();
    }
}

bool RunControl::promptToStop(bool *optionalPrompt) const
{
    QTC_ASSERT(isRunning(), return true);

    if (optionalPrompt && !*optionalPrompt)
        return true;

    const QString msg = tr("<html><head/><body><center><i>%1</i> is still running.<center/>"
                           "<center>Force it to quit?</center></body></html>").arg(displayName());
    return showPromptToStopDialog(tr("Application Still Running"), msg,
                                  tr("Force Quit"), tr("Keep Running"),
                                  optionalPrompt);
}

/*!
    \brief Utility to prompt to terminate application with checkable box.
*/

bool RunControl::showPromptToStopDialog(const QString &title,
                                        const QString &text,
                                        const QString &stopButtonText,
                                        const QString &cancelButtonText,
                                        bool *prompt) const
{
    QTC_ASSERT(isRunning(), return true);
    // Show a question message box where user can uncheck this
    // question for this class.
    Utils::CheckableMessageBox messageBox(Core::ICore::mainWindow());
    messageBox.setWindowTitle(title);
    messageBox.setText(text);
    messageBox.setStandardButtons(QDialogButtonBox::Yes|QDialogButtonBox::Cancel);
    if (!stopButtonText.isEmpty())
        messageBox.button(QDialogButtonBox::Yes)->setText(stopButtonText);
    if (!cancelButtonText.isEmpty())
        messageBox.button(QDialogButtonBox::Cancel)->setText(cancelButtonText);
    messageBox.setDefaultButton(QDialogButtonBox::Yes);
    if (prompt) {
        messageBox.setCheckBoxText(tr("Do not ask again"));
        messageBox.setChecked(false);
    } else {
        messageBox.setCheckBoxVisible(false);
    }
    messageBox.exec();
    const bool close = messageBox.clickedStandardButton() == QDialogButtonBox::Yes;
    if (close && prompt && messageBox.isChecked())
        *prompt = false;
    return close;
}

bool RunControl::sameRunConfiguration(const RunControl *other) const
{
    return other->m_runConfiguration.data() == m_runConfiguration.data();
}

void RunControl::bringApplicationToForeground(qint64 pid)
{
#ifdef Q_OS_MAC
    m_internalPid = pid;
    m_foregroundCount = 0;
    bringApplicationToForegroundInternal();
#else
    Q_UNUSED(pid)
#endif
}

void RunControl::bringApplicationToForegroundInternal()
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn;
    GetProcessForPID(m_internalPid, &psn);
    if (SetFrontProcess(&psn) == procNotFound && m_foregroundCount < 15) {
        // somehow the mac/carbon api says
        // "-600 no eligible process with specified process id"
        // if we call SetFrontProcess too early
        ++m_foregroundCount;
        QTimer::singleShot(200, this, SLOT(bringApplicationToForegroundInternal()));
        return;
    }
#endif
}

void RunControl::appendMessage(const QString &msg, Utils::OutputFormat format)
{
    emit appendMessage(this, msg, format);
}

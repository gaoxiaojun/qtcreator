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

#include "qt4runconfiguration.h"

#include "makestep.h"
#include "qt4nodes.h"
#include "qt4project.h"
#include "qt4target.h"
#include "qt4buildconfiguration.h"
#include "qt4projectmanagerconstants.h"
#include "qmakestep.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/variablemanager.h>
#include <coreplugin/idocument.h>
#include <coreplugin/helpmanager.h>
#include <projectexplorer/buildstep.h>
#include <projectexplorer/environmentwidget.h>
#include <projectexplorer/toolchain.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>
#include <utils/pathchooser.h>
#include <utils/detailswidget.h>
#include <utils/stringutils.h>
#include <utils/persistentsettings.h>
#include <qtsupport/qtoutputformatter.h>
#include <qtsupport/baseqtversion.h>
#include <qtsupport/profilereader.h>

#include <QFormLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QToolButton>
#include <QComboBox>
#include <QFileInfo>
#include <QDir>

using namespace Qt4ProjectManager::Internal;
using namespace Qt4ProjectManager;
using ProjectExplorer::LocalApplicationRunConfiguration;
using Utils::PersistentSettingsReader;
using Utils::PersistentSettingsWriter;

namespace {
const char * const QT4_RC_ID("Qt4ProjectManager.Qt4RunConfiguration");
const char * const QT4_RC_PREFIX("Qt4ProjectManager.Qt4RunConfiguration.");

const char * const COMMAND_LINE_ARGUMENTS_KEY("Qt4ProjectManager.Qt4RunConfiguration.CommandLineArguments");
const char * const PRO_FILE_KEY("Qt4ProjectManager.Qt4RunConfiguration.ProFile");
const char * const USE_TERMINAL_KEY("Qt4ProjectManager.Qt4RunConfiguration.UseTerminal");
const char * const USE_DYLD_IMAGE_SUFFIX_KEY("Qt4ProjectManager.Qt4RunConfiguration.UseDyldImageSuffix");
const char * const USER_ENVIRONMENT_CHANGES_KEY("Qt4ProjectManager.Qt4RunConfiguration.UserEnvironmentChanges");
const char * const BASE_ENVIRONMENT_BASE_KEY("Qt4ProjectManager.Qt4RunConfiguration.BaseEnvironmentBase");
const char * const USER_WORKING_DIRECTORY_KEY("Qt4ProjectManager.Qt4RunConfiguration.UserWorkingDirectory");

QString pathFromId(Core::Id id)
{
    QString idstr = QString::fromUtf8(id.name());
    const QString prefix = QLatin1String(QT4_RC_PREFIX);
    if (!idstr.startsWith(prefix))
        return QString();
    return idstr.mid(prefix.size());
}

} // namespace

//
// Qt4RunConfiguration
//

Qt4RunConfiguration::Qt4RunConfiguration(Qt4BaseTarget *parent, const QString &proFilePath) :
    LocalApplicationRunConfiguration(parent, Core::Id(QT4_RC_ID)),
    m_proFilePath(proFilePath),
    m_runMode(Gui),
    m_isUsingDyldImageSuffix(false),
    m_baseEnvironmentBase(Qt4RunConfiguration::BuildEnvironmentBase),
    m_parseSuccess(parent->qt4Project()->validParse(m_proFilePath)),
    m_parseInProgress(parent->qt4Project()->parseInProgress(m_proFilePath))
{
    ctor();
}

Qt4RunConfiguration::Qt4RunConfiguration(Qt4BaseTarget *parent, Qt4RunConfiguration *source) :
    LocalApplicationRunConfiguration(parent, source),
    m_commandLineArguments(source->m_commandLineArguments),
    m_proFilePath(source->m_proFilePath),
    m_runMode(source->m_runMode),
    m_isUsingDyldImageSuffix(source->m_isUsingDyldImageSuffix),
    m_userWorkingDirectory(source->m_userWorkingDirectory),
    m_userEnvironmentChanges(source->m_userEnvironmentChanges),
    m_baseEnvironmentBase(source->m_baseEnvironmentBase),
    m_parseSuccess(source->m_parseSuccess),
    m_parseInProgress(source->m_parseInProgress)
{
    ctor();
}

Qt4RunConfiguration::~Qt4RunConfiguration()
{
}

Qt4BaseTarget *Qt4RunConfiguration::qt4Target() const
{
    return static_cast<Qt4BaseTarget *>(target());
}

bool Qt4RunConfiguration::isEnabled() const
{
    return m_parseSuccess && !m_parseInProgress;
}

QString Qt4RunConfiguration::disabledReason() const
{
    if (m_parseInProgress)
        return tr("The .pro file is currently being parsed.");
    if (!m_parseSuccess)
        return tr("The .pro file could not be parsed.");
    return QString();
}

void Qt4RunConfiguration::proFileUpdated(Qt4ProjectManager::Qt4ProFileNode *pro, bool success, bool parseInProgress)
{
    if (m_proFilePath != pro->path()) {
        if (!parseInProgress) {
            // We depend on all .pro files for the LD_LIBRARY_PATH so we emit a signal for all .pro files
            // This can be optimized by checking whether LD_LIBRARY_PATH changed
            emit baseEnvironmentChanged();
        }
        return;
    }

    bool enabled = isEnabled();
    m_parseSuccess = success;
    m_parseInProgress = parseInProgress;
    if (enabled != isEnabled())
        emit enabledChanged();

    if (!parseInProgress) {
        emit effectiveTargetInformationChanged();
        emit baseEnvironmentChanged();
    }
}

void Qt4RunConfiguration::ctor()
{
    setDefaultDisplayName(defaultDisplayName());

    connect(qt4Target(), SIGNAL(environmentChanged()),
            this, SIGNAL(baseEnvironmentChanged()));
    connect(qt4Target()->qt4Project(), SIGNAL(proFileUpdated(Qt4ProjectManager::Qt4ProFileNode*,bool,bool)),
            this, SLOT(proFileUpdated(Qt4ProjectManager::Qt4ProFileNode*,bool,bool)));
}

//////
/// Qt4RunConfigurationWidget
/////

Qt4RunConfigurationWidget::Qt4RunConfigurationWidget(Qt4RunConfiguration *qt4RunConfiguration, QWidget *parent)
    : QWidget(parent),
    m_qt4RunConfiguration(qt4RunConfiguration),
    m_ignoreChange(false),
    m_usingDyldImageSuffix(0),
    m_isShown(false)
{
    QVBoxLayout *vboxTopLayout = new QVBoxLayout(this);
    vboxTopLayout->setMargin(0);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addStretch();
    m_disabledIcon = new QLabel(this);
    m_disabledIcon->setPixmap(QPixmap(QLatin1String(":/projectexplorer/images/compile_warning.png")));
    hl->addWidget(m_disabledIcon);
    m_disabledReason = new QLabel(this);
    m_disabledReason->setVisible(false);
    hl->addWidget(m_disabledReason);
    hl->addStretch();
    vboxTopLayout->addLayout(hl);

    m_detailsContainer = new Utils::DetailsWidget(this);
    m_detailsContainer->setState(Utils::DetailsWidget::NoSummary);
    vboxTopLayout->addWidget(m_detailsContainer);
    QWidget *detailsWidget = new QWidget(m_detailsContainer);
    m_detailsContainer->setWidget(detailsWidget);
    QFormLayout *toplayout = new QFormLayout(detailsWidget);
    toplayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    toplayout->setMargin(0);

    m_executableLineEdit = new QLineEdit(m_qt4RunConfiguration->executable(), this);
    m_executableLineEdit->setEnabled(false);
    toplayout->addRow(tr("Executable:"), m_executableLineEdit);

    QLabel *argumentsLabel = new QLabel(tr("Arguments:"), this);
    m_argumentsLineEdit = new QLineEdit(qt4RunConfiguration->rawCommandLineArguments(), this);
    argumentsLabel->setBuddy(m_argumentsLineEdit);
    toplayout->addRow(argumentsLabel, m_argumentsLineEdit);

    m_workingDirectoryEdit = new Utils::PathChooser(this);
    m_workingDirectoryEdit->setExpectedKind(Utils::PathChooser::Directory);
    m_workingDirectoryEdit->setPath(m_qt4RunConfiguration->baseWorkingDirectory());
    m_workingDirectoryEdit->setBaseDirectory(m_qt4RunConfiguration->target()->project()->projectDirectory());
    m_workingDirectoryEdit->setEnvironment(m_qt4RunConfiguration->environment());
    m_workingDirectoryEdit->setPromptDialogTitle(tr("Select Working Directory"));

    QToolButton *resetButton = new QToolButton(this);
    resetButton->setToolTip(tr("Reset to default"));
    resetButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_RESET)));

    QHBoxLayout *boxlayout = new QHBoxLayout();
    boxlayout->setMargin(0);
    boxlayout->addWidget(m_workingDirectoryEdit);
    boxlayout->addWidget(resetButton);
    toplayout->addRow(tr("Working directory:"), boxlayout);

    m_useTerminalCheck = new QCheckBox(tr("Run in terminal"), this);
    m_useTerminalCheck->setChecked(m_qt4RunConfiguration->runMode() == ProjectExplorer::LocalApplicationRunConfiguration::Console);
    toplayout->addRow(QString(), m_useTerminalCheck);
    m_useTerminalCheck->setVisible(qt4RunConfiguration->target()->id() != Core::Id(Constants::QT_SIMULATOR_TARGET_ID));

#ifdef Q_OS_MAC
    m_usingDyldImageSuffix = new QCheckBox(tr("Use debug version of frameworks (DYLD_IMAGE_SUFFIX=_debug)"), this);
    m_usingDyldImageSuffix->setChecked(m_qt4RunConfiguration->isUsingDyldImageSuffix());
    toplayout->addRow(QString(), m_usingDyldImageSuffix);
    connect(m_usingDyldImageSuffix, SIGNAL(toggled(bool)),
            this, SLOT(usingDyldImageSuffixToggled(bool)));
#endif

    QLabel *environmentLabel = new QLabel(this);
    environmentLabel->setText(tr("Run Environment"));
    QFont f = environmentLabel->font();
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() *1.2);
    environmentLabel->setFont(f);
    vboxTopLayout->addWidget(environmentLabel);

    QWidget *baseEnvironmentWidget = new QWidget(this);
    QHBoxLayout *baseEnvironmentLayout = new QHBoxLayout(baseEnvironmentWidget);
    baseEnvironmentLayout->setMargin(0);
    QLabel *label = new QLabel(tr("Base environment for this run configuration:"), this);
    baseEnvironmentLayout->addWidget(label);
    m_baseEnvironmentComboBox = new QComboBox(this);
    m_baseEnvironmentComboBox->addItems(QStringList()
                                        << tr("Clean Environment")
                                        << tr("System Environment")
                                        << tr("Build Environment"));
    m_baseEnvironmentComboBox->setCurrentIndex(qt4RunConfiguration->baseEnvironmentBase());
    connect(m_baseEnvironmentComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(baseEnvironmentSelected(int)));
    baseEnvironmentLayout->addWidget(m_baseEnvironmentComboBox);
    baseEnvironmentLayout->addStretch(10);

    m_environmentWidget = new ProjectExplorer::EnvironmentWidget(this, baseEnvironmentWidget);
    m_environmentWidget->setBaseEnvironment(m_qt4RunConfiguration->baseEnvironment());
    m_environmentWidget->setBaseEnvironmentText(m_qt4RunConfiguration->baseEnvironmentText());
    m_environmentWidget->setUserChanges(m_qt4RunConfiguration->userEnvironmentChanges());
    m_environmentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vboxTopLayout->addWidget(m_environmentWidget);

    runConfigurationEnabledChange();

    connect(m_workingDirectoryEdit, SIGNAL(changed(QString)),
            this, SLOT(workDirectoryEdited()));

    connect(resetButton, SIGNAL(clicked()),
            this, SLOT(workingDirectoryReseted()));

    connect(m_argumentsLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(argumentsEdited(QString)));
    connect(m_useTerminalCheck, SIGNAL(toggled(bool)),
            this, SLOT(termToggled(bool)));

    connect(m_environmentWidget, SIGNAL(userChangesChanged()),
            this, SLOT(userChangesEdited()));

    connect(qt4RunConfiguration, SIGNAL(baseWorkingDirectoryChanged(QString)),
            this, SLOT(workingDirectoryChanged(QString)));

    connect(qt4RunConfiguration, SIGNAL(commandLineArgumentsChanged(QString)),
            this, SLOT(commandLineArgumentsChanged(QString)));
    connect(qt4RunConfiguration, SIGNAL(runModeChanged(ProjectExplorer::LocalApplicationRunConfiguration::RunMode)),
            this, SLOT(runModeChanged(ProjectExplorer::LocalApplicationRunConfiguration::RunMode)));
    connect(qt4RunConfiguration, SIGNAL(usingDyldImageSuffixChanged(bool)),
            this, SLOT(usingDyldImageSuffixChanged(bool)));
    connect(qt4RunConfiguration, SIGNAL(effectiveTargetInformationChanged()),
            this, SLOT(effectiveTargetInformationChanged()), Qt::QueuedConnection);

    connect(qt4RunConfiguration, SIGNAL(userEnvironmentChangesChanged(QList<Utils::EnvironmentItem>)),
            this, SLOT(userEnvironmentChangesChanged(QList<Utils::EnvironmentItem>)));

    connect(qt4RunConfiguration, SIGNAL(baseEnvironmentChanged()),
            this, SLOT(baseEnvironmentChanged()));

    connect(qt4RunConfiguration, SIGNAL(enabledChanged()),
            this, SLOT(runConfigurationEnabledChange()));
}

Qt4RunConfigurationWidget::~Qt4RunConfigurationWidget()
{
}

void Qt4RunConfigurationWidget::baseEnvironmentSelected(int index)
{
    m_ignoreChange = true;
    m_qt4RunConfiguration->setBaseEnvironmentBase(Qt4RunConfiguration::BaseEnvironmentBase(index));

    m_environmentWidget->setBaseEnvironment(m_qt4RunConfiguration->baseEnvironment());
    m_environmentWidget->setBaseEnvironmentText(m_qt4RunConfiguration->baseEnvironmentText());
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::baseEnvironmentChanged()
{
    if (m_ignoreChange)
        return;

    m_baseEnvironmentComboBox->setCurrentIndex(m_qt4RunConfiguration->baseEnvironmentBase());
    m_environmentWidget->setBaseEnvironment(m_qt4RunConfiguration->baseEnvironment());
    m_environmentWidget->setBaseEnvironmentText(m_qt4RunConfiguration->baseEnvironmentText());
}

void Qt4RunConfigurationWidget::userEnvironmentChangesChanged(const QList<Utils::EnvironmentItem> &userChanges)
{
    if (m_ignoreChange)
        return;
    m_environmentWidget->setUserChanges(userChanges);
}

void Qt4RunConfigurationWidget::userChangesEdited()
{
    m_ignoreChange = true;
    m_qt4RunConfiguration->setUserEnvironmentChanges(m_environmentWidget->userChanges());
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::runConfigurationEnabledChange()
{
    bool enabled = m_qt4RunConfiguration->isEnabled();
    m_detailsContainer->setEnabled(enabled);
    m_environmentWidget->setEnabled(enabled);
    m_disabledIcon->setVisible(!enabled);
    m_disabledReason->setVisible(!enabled);
    m_disabledReason->setText(m_qt4RunConfiguration->disabledReason());
}

void Qt4RunConfigurationWidget::workDirectoryEdited()
{
    if (m_ignoreChange)
        return;
    m_ignoreChange = true;
    m_qt4RunConfiguration->setBaseWorkingDirectory(m_workingDirectoryEdit->rawPath());
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::workingDirectoryReseted()
{
    // This emits a signal connected to workingDirectoryChanged()
    // that sets the m_workingDirectoryEdit
    m_qt4RunConfiguration->setBaseWorkingDirectory(QString());
}

void Qt4RunConfigurationWidget::argumentsEdited(const QString &args)
{
    m_ignoreChange = true;
    m_qt4RunConfiguration->setCommandLineArguments(args);
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::termToggled(bool on)
{
    m_ignoreChange = true;
    m_qt4RunConfiguration->setRunMode(on ? LocalApplicationRunConfiguration::Console
                                         : LocalApplicationRunConfiguration::Gui);
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::usingDyldImageSuffixToggled(bool state)
{
    m_ignoreChange = true;
    m_qt4RunConfiguration->setUsingDyldImageSuffix(state);
    m_ignoreChange = false;
}

void Qt4RunConfigurationWidget::workingDirectoryChanged(const QString &workingDirectory)
{
    if (!m_ignoreChange)
        m_workingDirectoryEdit->setPath(workingDirectory);
}

void Qt4RunConfigurationWidget::commandLineArgumentsChanged(const QString &args)
{
    if (m_ignoreChange)
        return;
    m_argumentsLineEdit->setText(args);
}

void Qt4RunConfigurationWidget::runModeChanged(LocalApplicationRunConfiguration::RunMode runMode)
{
    if (!m_ignoreChange)
        m_useTerminalCheck->setChecked(runMode == LocalApplicationRunConfiguration::Console);
}

void Qt4RunConfigurationWidget::usingDyldImageSuffixChanged(bool state)
{
    if (!m_ignoreChange && m_usingDyldImageSuffix)
        m_usingDyldImageSuffix->setChecked(state);
}

void Qt4RunConfigurationWidget::effectiveTargetInformationChanged()
{
    if (m_isShown) {
        m_executableLineEdit->setText(QDir::toNativeSeparators(m_qt4RunConfiguration->executable()));
        m_ignoreChange = true;
        m_workingDirectoryEdit->setPath(QDir::toNativeSeparators(m_qt4RunConfiguration->baseWorkingDirectory()));
        m_ignoreChange = false;
    }
}

void Qt4RunConfigurationWidget::showEvent(QShowEvent *event)
{
    m_isShown = true;
    effectiveTargetInformationChanged();
    QWidget::showEvent(event);
}

void Qt4RunConfigurationWidget::hideEvent(QHideEvent *event)
{
    m_isShown = false;
    QWidget::hideEvent(event);
}

QWidget *Qt4RunConfiguration::createConfigurationWidget()
{
    return new Qt4RunConfigurationWidget(this, 0);
}

QVariantMap Qt4RunConfiguration::toMap() const
{
    const QDir projectDir = QDir(target()->project()->projectDirectory());
    QVariantMap map(LocalApplicationRunConfiguration::toMap());
    map.insert(QLatin1String(COMMAND_LINE_ARGUMENTS_KEY), m_commandLineArguments);
    map.insert(QLatin1String(PRO_FILE_KEY), projectDir.relativeFilePath(m_proFilePath));
    map.insert(QLatin1String(USE_TERMINAL_KEY), m_runMode == Console);
    map.insert(QLatin1String(USE_DYLD_IMAGE_SUFFIX_KEY), m_isUsingDyldImageSuffix);
    map.insert(QLatin1String(USER_ENVIRONMENT_CHANGES_KEY), Utils::EnvironmentItem::toStringList(m_userEnvironmentChanges));
    map.insert(QLatin1String(BASE_ENVIRONMENT_BASE_KEY), m_baseEnvironmentBase);
    map.insert(QLatin1String(USER_WORKING_DIRECTORY_KEY), m_userWorkingDirectory);
    return map;
}

bool Qt4RunConfiguration::fromMap(const QVariantMap &map)
{
    const QDir projectDir = QDir(target()->project()->projectDirectory());
    m_commandLineArguments = map.value(QLatin1String(COMMAND_LINE_ARGUMENTS_KEY)).toString();
    m_proFilePath = QDir::cleanPath(projectDir.filePath(map.value(QLatin1String(PRO_FILE_KEY)).toString()));
    m_runMode = map.value(QLatin1String(USE_TERMINAL_KEY), false).toBool() ? Console : Gui;
    m_isUsingDyldImageSuffix = map.value(QLatin1String(USE_DYLD_IMAGE_SUFFIX_KEY), false).toBool();

    m_userWorkingDirectory = map.value(QLatin1String(USER_WORKING_DIRECTORY_KEY)).toString();

    m_userEnvironmentChanges = Utils::EnvironmentItem::fromStringList(map.value(QLatin1String(USER_ENVIRONMENT_CHANGES_KEY)).toStringList());
    m_baseEnvironmentBase = static_cast<BaseEnvironmentBase>(map.value(QLatin1String(BASE_ENVIRONMENT_BASE_KEY), static_cast<int>(Qt4RunConfiguration::BuildEnvironmentBase)).toInt());

    m_parseSuccess = qt4Target()->qt4Project()->validParse(m_proFilePath);
    m_parseInProgress = qt4Target()->qt4Project()->parseInProgress(m_proFilePath);

    return RunConfiguration::fromMap(map);
}

QString Qt4RunConfiguration::executable() const
{
    Qt4Project *pro = qt4Target()->qt4Project();
    TargetInformation ti = pro->rootQt4ProjectNode()->targetInformation(m_proFilePath);
    if (!ti.valid)
        return QString();
    return ti.executable;
}

LocalApplicationRunConfiguration::RunMode Qt4RunConfiguration::runMode() const
{
    return m_runMode;
}

bool Qt4RunConfiguration::isUsingDyldImageSuffix() const
{
    return m_isUsingDyldImageSuffix;
}

void Qt4RunConfiguration::setUsingDyldImageSuffix(bool state)
{
    m_isUsingDyldImageSuffix = state;
    emit usingDyldImageSuffixChanged(state);
}

QString Qt4RunConfiguration::workingDirectory() const
{
    return QDir::cleanPath(environment().expandVariables(
                Utils::expandMacros(baseWorkingDirectory(), macroExpander())));
}

QString Qt4RunConfiguration::baseWorkingDirectory() const
{
    // if the user overrode us, then return his working directory
    if (!m_userWorkingDirectory.isEmpty())
        return m_userWorkingDirectory;

    // else what the pro file reader tells us
    Qt4Project *pro = qt4Target()->qt4Project();
    TargetInformation ti = pro->rootQt4ProjectNode()->targetInformation(m_proFilePath);
    if (!ti.valid)
        return QString();
    return ti.workingDir;
}

QString Qt4RunConfiguration::commandLineArguments() const
{
    return Utils::QtcProcess::expandMacros(m_commandLineArguments, macroExpander());
}

QString Qt4RunConfiguration::rawCommandLineArguments() const
{
    return m_commandLineArguments;
}

QString Qt4RunConfiguration::baseEnvironmentText() const
{
    if (m_baseEnvironmentBase == Qt4RunConfiguration::CleanEnvironmentBase)
        return tr("Clean Environment");
    else  if (m_baseEnvironmentBase == Qt4RunConfiguration::SystemEnvironmentBase)
        return tr("System Environment");
    else  if (m_baseEnvironmentBase == Qt4RunConfiguration::BuildEnvironmentBase)
        return tr("Build Environment");
    return QString();
}

Utils::Environment Qt4RunConfiguration::baseEnvironment() const
{
    Utils::Environment env;
    if (m_baseEnvironmentBase == Qt4RunConfiguration::CleanEnvironmentBase) {
        // Nothing
    } else  if (m_baseEnvironmentBase == Qt4RunConfiguration::SystemEnvironmentBase) {
        env = Utils::Environment::systemEnvironment();
    } else  if (m_baseEnvironmentBase == Qt4RunConfiguration::BuildEnvironmentBase) {
        env = target()->activeBuildConfiguration()->environment();
    }
    if (m_isUsingDyldImageSuffix) {
        env.set(QLatin1String("DYLD_IMAGE_SUFFIX"), QLatin1String("_debug"));
    }

    // The user could be linking to a library found via a -L/some/dir switch
    // to find those libraries while actually running we explicitly prepend those
    // dirs to the library search path
    const Qt4ProFileNode *node = qt4Target()->qt4Project()->rootQt4ProjectNode()->findProFileFor(m_proFilePath);
    if (node) {
        const QStringList libDirectories = node->variableValue(LibDirectoriesVar);
        if (!libDirectories.isEmpty()) {
            const QString proDirectory = node->buildDir();
            foreach (QString dir, libDirectories) {
                // Fix up relative entries like "LIBS+=-L.."
                const QFileInfo fi(dir);
                if (!fi.isAbsolute())
                    dir = QDir::cleanPath(proDirectory + QLatin1Char('/') + dir);
                env.prependOrSetLibrarySearchPath(dir);
            } // foreach
        } // libDirectories
    } // node

    QtSupport::BaseQtVersion *qtVersion = qt4Target()->activeQt4BuildConfiguration()->qtVersion();
    if (qtVersion)
        env.prependOrSetLibrarySearchPath(qtVersion->versionInfo().value(QLatin1String("QT_INSTALL_LIBS")));
    return env;
}

Utils::Environment Qt4RunConfiguration::environment() const
{
    Utils::Environment env = baseEnvironment();
    env.modify(userEnvironmentChanges());
    return env;
}

QList<Utils::EnvironmentItem> Qt4RunConfiguration::userEnvironmentChanges() const
{
    return m_userEnvironmentChanges;
}

void Qt4RunConfiguration::setUserEnvironmentChanges(const QList<Utils::EnvironmentItem> &diff)
{
    if (m_userEnvironmentChanges != diff) {
        m_userEnvironmentChanges = diff;
        emit userEnvironmentChangesChanged(diff);
    }
}

void Qt4RunConfiguration::setBaseWorkingDirectory(const QString &wd)
{
    const QString &oldWorkingDirectory = workingDirectory();

    m_userWorkingDirectory = wd;

    const QString &newWorkingDirectory = workingDirectory();
    if (oldWorkingDirectory != newWorkingDirectory)
        emit baseWorkingDirectoryChanged(newWorkingDirectory);
}

void Qt4RunConfiguration::setCommandLineArguments(const QString &argumentsString)
{
    m_commandLineArguments = argumentsString;
    emit commandLineArgumentsChanged(argumentsString);
}

void Qt4RunConfiguration::setRunMode(RunMode runMode)
{
    m_runMode = runMode;
    emit runModeChanged(runMode);
}

QString Qt4RunConfiguration::proFilePath() const
{
    return m_proFilePath;
}

QString Qt4RunConfiguration::dumperLibrary() const
{
    QtSupport::BaseQtVersion *version = qt4Target()->activeQt4BuildConfiguration()->qtVersion();
    if (version)
        return version->gdbDebuggingHelperLibrary();
    return QString();
}

QStringList Qt4RunConfiguration::dumperLibraryLocations() const
{
    QtSupport::BaseQtVersion *version = qt4Target()->activeQt4BuildConfiguration()->qtVersion();
    if (version)
        return version->debuggingHelperLibraryLocations();
    return QStringList();
}

QString Qt4RunConfiguration::defaultDisplayName()
{
    QString defaultName;
    if (!m_proFilePath.isEmpty())
        defaultName = QFileInfo(m_proFilePath).completeBaseName();
    else
        defaultName = tr("Qt4 Run Configuration");
    return defaultName;
}

void Qt4RunConfiguration::setBaseEnvironmentBase(BaseEnvironmentBase env)
{
    if (m_baseEnvironmentBase == env)
        return;
    m_baseEnvironmentBase = env;
    emit baseEnvironmentChanged();
}

Qt4RunConfiguration::BaseEnvironmentBase Qt4RunConfiguration::baseEnvironmentBase() const
{
    return m_baseEnvironmentBase;
}

Utils::OutputFormatter *Qt4RunConfiguration::createOutputFormatter() const
{
    return new QtSupport::QtOutputFormatter(qt4Target()->qt4Project());
}

///
/// Qt4RunConfigurationFactory
/// This class is used to restore run settings (saved in .user files)
///

Qt4RunConfigurationFactory::Qt4RunConfigurationFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{
}

Qt4RunConfigurationFactory::~Qt4RunConfigurationFactory()
{
}

bool Qt4RunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const Core::Id id) const
{
    Qt4BaseTarget *t = qobject_cast<Qt4BaseTarget *>(parent);
    if (!t)
        return false;
    if (t->id() != Core::Id(Constants::DESKTOP_TARGET_ID)
            && t->id() != Core::Id(Constants::QT_SIMULATOR_TARGET_ID))
        return false;
    return t->qt4Project()->hasApplicationProFile(pathFromId(id));
}

ProjectExplorer::RunConfiguration *Qt4RunConfigurationFactory::create(ProjectExplorer::Target *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;
    Qt4BaseTarget *t = static_cast<Qt4BaseTarget *>(parent);
    return new Qt4RunConfiguration(t, pathFromId(id));
}

bool Qt4RunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    if (!qobject_cast<Qt4BaseTarget *>(parent))
        return false;
    if (parent->id() != Core::Id(Constants::DESKTOP_TARGET_ID)
            && parent->id() != Core::Id(Constants::QT_SIMULATOR_TARGET_ID))
        return false;
    QString id = QString::fromLatin1(ProjectExplorer::idFromMap(map).name());
    return id.startsWith(QLatin1String(QT4_RC_ID));
}

ProjectExplorer::RunConfiguration *Qt4RunConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    Qt4BaseTarget *t = static_cast<Qt4BaseTarget *>(parent);
    Qt4RunConfiguration *rc = new Qt4RunConfiguration(t, QString());
    if (rc->fromMap(map))
        return rc;

    delete rc;
    return 0;
}

bool Qt4RunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *Qt4RunConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    Qt4BaseTarget *t = static_cast<Qt4BaseTarget *>(parent);
    Qt4RunConfiguration *old = static_cast<Qt4RunConfiguration *>(source);
    return new Qt4RunConfiguration(t, old);
}

QList<Core::Id> Qt4RunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    QList<Core::Id> result;
    Qt4BaseTarget *t = qobject_cast<Qt4BaseTarget *>(parent);
    if (!t)
        return result;
    if (t->id() != Core::Id(Constants::DESKTOP_TARGET_ID)
            && t->id() != Core::Id(Constants::QT_SIMULATOR_TARGET_ID))
        return result;
    QStringList proFiles = t->qt4Project()->applicationProFilePathes(QLatin1String(QT4_RC_PREFIX));
    foreach (const QString &pf, proFiles)
        result << Core::Id(pf.toUtf8().constData());
    return result;
}

QString Qt4RunConfigurationFactory::displayNameForId(const Core::Id id) const
{
    return QFileInfo(pathFromId(id)).completeBaseName();
}

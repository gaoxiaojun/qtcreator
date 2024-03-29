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

#include "s60deployconfiguration.h"
#include "s60deployconfigurationwidget.h"
#include "s60manager.h"
#include "qt4project.h"
#include "qt4target.h"
#include "qt4nodes.h"
#include "qt4projectmanagerconstants.h"
#include "qt4buildconfiguration.h"
#include "qt4symbiantarget.h"
#include "s60createpackagestep.h"
#include "s60deploystep.h"
#include "symbianidevice.h"
#include "symbianidevicefactory.h"

#include <utils/qtcassert.h>
#include <symbianutils/symbiandevicemanager.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/devicesupport/devicemanager.h>

#include <QFileInfo>

using namespace ProjectExplorer;
using namespace Qt4ProjectManager;
using namespace Qt4ProjectManager::Internal;

namespace {
const char S60_DC_PREFIX[] = "Qt4ProjectManager.S60DeployConfiguration.";

const char INSTALLATION_DRIVE_LETTER_KEY[] = "Qt4ProjectManager.S60DeployConfiguration.InstallationDriveLetter";
const char SILENT_INSTALL_KEY[] = "Qt4ProjectManager.S60DeployConfiguration.SilentInstall";

QString pathFromId(Core::Id id)
{
    QString idstr = QString::fromUtf8(id.name());
    const QString prefix = QLatin1String(S60_DC_PREFIX);
    if (!idstr.startsWith(prefix))
        return QString();
    return idstr.mid(prefix.size());
}

} // namespace

// ======== S60DeployConfiguration

S60DeployConfiguration::S60DeployConfiguration(Target *parent) :
    DeployConfiguration(parent,  Core::Id(S60_DEPLOYCONFIGURATION_ID)),
    m_activeBuildConfiguration(0),
    m_installationDrive('C'),
    m_silentInstall(true)
{
    ctor();

    ProjectExplorer::DeviceManager *dm = ProjectExplorer::DeviceManager::instance();
    IDevice::ConstPtr defaultDevice
            = dm->defaultDevice(Internal::SymbianIDeviceFactory::deviceType());
    if (defaultDevice)
        m_deviceId = defaultDevice->id();
}

S60DeployConfiguration::S60DeployConfiguration(Target *target, S60DeployConfiguration *source) :
    DeployConfiguration(target, source),
    m_activeBuildConfiguration(0),
    m_deviceId(source->m_deviceId),
    m_installationDrive(source->m_installationDrive),
    m_silentInstall(source->m_silentInstall)
{
    ctor();
}

void S60DeployConfiguration::ctor()
{
    setDefaultDisplayName(defaultDisplayName());
    // TODO disable S60 Deploy Configuration while parsing
    // requires keeping track of the parsing state of the project
    connect(qt4Target()->qt4Project(), SIGNAL(proFileUpdated(Qt4ProjectManager::Qt4ProFileNode*,bool,bool)),
            this, SLOT(slotTargetInformationChanged(Qt4ProjectManager::Qt4ProFileNode*,bool,bool)));
    connect(qt4Target(), SIGNAL(activeBuildConfigurationChanged(ProjectExplorer::BuildConfiguration*)),
            this, SLOT(updateActiveBuildConfiguration(ProjectExplorer::BuildConfiguration*)));
    connect(qt4Target(), SIGNAL(activeRunConfigurationChanged(ProjectExplorer::RunConfiguration*)),
            this, SLOT(updateActiveRunConfiguration(ProjectExplorer::RunConfiguration*)));
    updateActiveBuildConfiguration(qt4Target()->activeBuildConfiguration());
}

S60DeployConfiguration::~S60DeployConfiguration()
{ }

ProjectExplorer::DeployConfigurationWidget *S60DeployConfiguration::configurationWidget() const
{
    return new S60DeployConfigurationWidget();
}

void S60DeployConfiguration::slotTargetInformationChanged(Qt4ProjectManager::Qt4ProFileNode*,bool success, bool parseInProgress)
{
    Q_UNUSED(success)
    if (!parseInProgress)
        emit targetInformationChanged();
}

bool S60DeployConfiguration::isStaticLibrary(const Qt4ProFileNode &projectNode) const
{
    if (projectNode.projectType() == LibraryTemplate) {
        const QStringList &config(projectNode.variableValue(ConfigVar));
        if (config.contains(QLatin1String("static")) || config.contains(QLatin1String("staticlib")))
            return true;
    }
    return false;
}

bool S60DeployConfiguration::isApplication(const Qt4ProFileNode &projectNode) const
{
    return projectNode.projectType() == ApplicationTemplate;
}

bool S60DeployConfiguration::hasSisPackage(const Qt4ProFileNode &projectNode) const
{
    return projectNode.isDeployable();
}

QStringList S60DeployConfiguration::signedPackages() const
{
    QList<Qt4ProFileNode *> list = qt4Target()->qt4Project()->allProFiles();
    QStringList result;
    foreach (Qt4ProFileNode *node, list) {
        if (!hasSisPackage(*node))
            continue;
        TargetInformation ti = node->targetInformation();
        if (ti.valid)
            result << ti.buildDir + QLatin1Char('/') + createPackageName(ti.target);
    }
    return result;
}

QString S60DeployConfiguration::createPackageName(const QString &baseName) const
{
    QString name(baseName);
    name += runSmartInstaller() ? QLatin1String("_installer") : QLatin1String("");
    name += isSigned() ? QLatin1String("") : QLatin1String("_unsigned");
    name += QLatin1String(".sis");
    return name;
}

SymbianIDevice::ConstPtr S60DeployConfiguration::device() const
{
    return ProjectExplorer::DeviceManager::instance()->find(m_deviceId).dynamicCast<const SymbianIDevice>();
}

QStringList S60DeployConfiguration::packageFileNamesWithTargetInfo() const
{
    QList<Qt4ProFileNode *> leafs = qt4Target()->qt4Project()->allProFiles();
    QStringList result;
    foreach (Qt4ProFileNode *qt4ProFileNode, leafs) {
        if (!hasSisPackage(*qt4ProFileNode))
            continue;
        TargetInformation ti = qt4ProFileNode->targetInformation();
        if (!ti.valid)
            continue;
        QString baseFileName = ti.buildDir + QLatin1Char('/') + ti.target;
        baseFileName += QLatin1Char('_')
                + (isDebug() ? QLatin1String("debug") : QLatin1String("release"))
                + QLatin1Char('-') + S60Manager::platform(qt4Target()->activeBuildConfiguration()->toolChain()) + QLatin1String(".sis");
        result << baseFileName;
    }
    return result;
}

QStringList S60DeployConfiguration::packageTemplateFileNames() const
{
    QList<Qt4ProFileNode *> list = qt4Target()->qt4Project()->allProFiles();
    QStringList result;
    foreach (Qt4ProFileNode *node, list) {
        if (!hasSisPackage(*node))
            continue;
        TargetInformation ti = node->targetInformation();
        if (ti.valid)
            result << ti.buildDir + QLatin1Char('/') + ti.target + QLatin1String("_template.pkg");
    }
    return result;
}

QStringList S60DeployConfiguration::appPackageTemplateFileNames() const
{
    QList<Qt4ProFileNode *> list = qt4Target()->qt4Project()->allProFiles();
    QStringList result;
    foreach (Qt4ProFileNode *node, list) {
        if (!hasSisPackage(*node))
            continue;
        TargetInformation ti = node->targetInformation();
        if (ti.valid)
            result << ti.buildDir + QLatin1Char('/') + ti.target + QLatin1String("_template.pkg");
    }
    return result;
}

bool S60DeployConfiguration::runSmartInstaller() const
{
    DeployConfiguration *dc = target()->activeDeployConfiguration();
    QTC_ASSERT(dc, return false);
    BuildStepList *bsl = dc->stepList();
    QTC_ASSERT(bsl, return false);
    QList<BuildStep *> steps = bsl->steps();
    foreach (const BuildStep *step, steps) {
        if (const S60CreatePackageStep *packageStep = qobject_cast<const S60CreatePackageStep *>(step)) {
            return packageStep->createsSmartInstaller();
        }
    }
    return false;
}

bool S60DeployConfiguration::isSigned() const
{
    DeployConfiguration *dc = target()->activeDeployConfiguration();
    QTC_ASSERT(dc, return false);
    BuildStepList *bsl = dc->stepList();
    QTC_ASSERT(bsl, return false);
    QList<BuildStep *> steps = bsl->steps();
    foreach (const BuildStep *step, steps) {
        if (const S60CreatePackageStep *packageStep = qobject_cast<const S60CreatePackageStep *>(step)) {
            return packageStep->signingMode() != S60CreatePackageStep::NotSigned;
        }
    }
    return false;
}

ProjectExplorer::ToolChain *S60DeployConfiguration::toolChain() const
{
    if (Qt4BuildConfiguration *bc = qobject_cast<Qt4BuildConfiguration *>(target()->activeBuildConfiguration()))
        return bc->toolChain();
    return 0;
}

bool S60DeployConfiguration::isDebug() const
{
    const Qt4BuildConfiguration *qt4bc = qt4Target()->activeQt4BuildConfiguration();
    return (qt4bc->qmakeBuildConfiguration() & QtSupport::BaseQtVersion::DebugBuild);
}

QString S60DeployConfiguration::symbianTarget() const
{
    return isDebug() ? QLatin1String("udeb") : QLatin1String("urel");
}

const QtSupport::BaseQtVersion *S60DeployConfiguration::qtVersion() const
{
    if (const Qt4BuildConfiguration *qt4bc = qt4Target()->activeQt4BuildConfiguration())
        return qt4bc->qtVersion();
    return 0;
}

void S60DeployConfiguration::updateActiveBuildConfiguration(ProjectExplorer::BuildConfiguration *buildConfiguration)
{
    if (m_activeBuildConfiguration)
        disconnect(m_activeBuildConfiguration, SIGNAL(s60CreatesSmartInstallerChanged()),
                   this, SIGNAL(targetInformationChanged()));
    m_activeBuildConfiguration = buildConfiguration;
    if (m_activeBuildConfiguration)
        connect(m_activeBuildConfiguration, SIGNAL(s60CreatesSmartInstallerChanged()),
                this, SIGNAL(targetInformationChanged()));
}

void S60DeployConfiguration::updateActiveRunConfiguration(ProjectExplorer::RunConfiguration *runConfiguration)
{
    Q_UNUSED(runConfiguration);
    setDefaultDisplayName(defaultDisplayName());
}

QVariantMap S60DeployConfiguration::toMap() const
{
    QVariantMap map(ProjectExplorer::DeployConfiguration::toMap());
    map.insert(QLatin1String(INSTALLATION_DRIVE_LETTER_KEY), QChar(QLatin1Char(m_installationDrive)));
    map.insert(QLatin1String(SILENT_INSTALL_KEY), QVariant(m_silentInstall));

    return map;
}

QString S60DeployConfiguration::defaultDisplayName() const
{
    QList<Qt4ProFileNode *> list = qt4Target()->qt4Project()->allProFiles();
    foreach (Qt4ProFileNode *node, list) {
        TargetInformation ti = node->targetInformation();
        if (ti.valid && !ti.buildDir.isEmpty())
            return tr("Deploy %1 to Symbian device").arg(QFileInfo(ti.buildDir).completeBaseName());
    }
    return tr("Deploy to Symbian device");
}

bool S60DeployConfiguration::fromMap(const QVariantMap &map)
{
    if (!DeployConfiguration::fromMap(map))
        return false;
    m_installationDrive = map.value(QLatin1String(INSTALLATION_DRIVE_LETTER_KEY), QChar(QLatin1Char('C')))
                          .toChar().toAscii();
    m_silentInstall = map.value(QLatin1String(SILENT_INSTALL_KEY), QVariant(true)).toBool();

    setDefaultDisplayName(defaultDisplayName());
    return true;
}

Qt4SymbianTarget *S60DeployConfiguration::qt4Target() const
{
    return static_cast<Qt4SymbianTarget *>(target());
}

char S60DeployConfiguration::installationDrive() const
{
    return m_installationDrive;
}

void S60DeployConfiguration::setInstallationDrive(char drive)
{
    if (m_installationDrive == drive)
        return;
    m_installationDrive = drive;
    emit installationDriveChanged();
}

bool S60DeployConfiguration::silentInstall() const
{
    return m_silentInstall;
}

void S60DeployConfiguration::setSilentInstall(bool silent)
{
    m_silentInstall = silent;
}

void S60DeployConfiguration::setAvailableDeviceDrives(QList<DeviceDrive> drives)
{
    m_availableDeviceDrives = drives;
    emit availableDeviceDrivesChanged();
}

const QList<S60DeployConfiguration::DeviceDrive> &S60DeployConfiguration::availableDeviceDrives() const
{
    return m_availableDeviceDrives;
}

// ======== S60DeployConfigurationFactory

S60DeployConfigurationFactory::S60DeployConfigurationFactory(QObject *parent) :
    DeployConfigurationFactory(parent)
{
}

S60DeployConfigurationFactory::~S60DeployConfigurationFactory()
{
}

QList<Core::Id> S60DeployConfigurationFactory::availableCreationIds(Target *parent) const
{
    QList<Core::Id> result;
    Qt4SymbianTarget *target = qobject_cast<Qt4SymbianTarget *>(parent);
    if (!target || target->id() != Core::Id(Constants::S60_DEVICE_TARGET_ID))
        return result;

    QStringList proFiles = target->qt4Project()->applicationProFilePathes(QLatin1String(S60_DC_PREFIX));
    foreach (const QString &pf, proFiles)
        result << Core::Id(pf.toUtf8().constData());
    return result;
}

QString S60DeployConfigurationFactory::displayNameForId(const Core::Id id) const
{
    if (!pathFromId(id).isEmpty())
        return tr("%1 on Symbian Device").arg(QFileInfo(pathFromId(id)).completeBaseName());
    return QString();
}

DeployConfiguration *S60DeployConfigurationFactory::create(Target *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;

    Qt4SymbianTarget *t = static_cast<Qt4SymbianTarget *>(parent);
    S60DeployConfiguration *dc = new S60DeployConfiguration(t);

    dc->setDefaultDisplayName(tr("Deploy to Symbian device"));
    dc->stepList()->insertStep(0, new S60CreatePackageStep(dc->stepList()));
    dc->stepList()->insertStep(1, new S60DeployStep(dc->stepList()));
    return dc;
}

bool S60DeployConfigurationFactory::canCreate(Target *parent, const Core::Id id) const
{
    Qt4SymbianTarget * t = qobject_cast<Qt4SymbianTarget *>(parent);
    if (!t || t->id() != Core::Id(Constants::S60_DEVICE_TARGET_ID)
            || !QString::fromUtf8(id.name()).startsWith(QLatin1String(S60_DEPLOYCONFIGURATION_ID)))
        return false;
    return true;
}

bool S60DeployConfigurationFactory::canRestore(Target *parent, const QVariantMap& map) const
{
    return canCreate(parent, idFromMap(map));
}

DeployConfiguration *S60DeployConfigurationFactory::restore(Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    Qt4SymbianTarget *t = static_cast<Qt4SymbianTarget *>(parent);
    S60DeployConfiguration *dc = new S60DeployConfiguration(t);
    if (dc->fromMap(map))
        return dc;

    delete dc;
    return 0;
}

bool S60DeployConfigurationFactory::canClone(Target *parent, DeployConfiguration *source) const
{
    if (!qobject_cast<Qt4SymbianTarget *>(parent))
        return false;
    return source->id() == Core::Id(S60_DEPLOYCONFIGURATION_ID);
}

DeployConfiguration *S60DeployConfigurationFactory::clone(Target *parent, DeployConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    Qt4SymbianTarget *t = static_cast<Qt4SymbianTarget *>(parent);
    S60DeployConfiguration * old = static_cast<S60DeployConfiguration *>(source);
    return new S60DeployConfiguration(t, old);
}

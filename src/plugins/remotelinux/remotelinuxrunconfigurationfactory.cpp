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
#include "remotelinuxrunconfigurationfactory.h"

#include "remotelinuxdeployconfigurationfactory.h"
#include "remotelinuxrunconfiguration.h"
#include "remotelinuxutils.h"

#include <qt4projectmanager/qt4project.h>
#include <qt4projectmanager/qt4target.h>
#include <utils/qtcassert.h>

#include <QFileInfo>
#include <QString>
#include <QStringList>

using namespace ProjectExplorer;
using namespace Qt4ProjectManager;

namespace RemoteLinux {
namespace Internal {

namespace {
QString pathFromId(Core::Id id)
{
    QString idStr = QString::fromUtf8(id.name());
    if (!idStr.startsWith(RemoteLinuxRunConfiguration::Id))
        return QString();
    return idStr.mid(RemoteLinuxRunConfiguration::Id.size());
}

} // namespace

RemoteLinuxRunConfigurationFactory::RemoteLinuxRunConfigurationFactory(QObject *parent)
    : IRunConfigurationFactory(parent)
{
}

RemoteLinuxRunConfigurationFactory::~RemoteLinuxRunConfigurationFactory()
{
}

bool RemoteLinuxRunConfigurationFactory::canCreate(Target *parent,
    const Core::Id id) const
{
    if (!QString::fromUtf8(id.name()).startsWith(RemoteLinuxRunConfiguration::Id))
        return false;
    return qobject_cast<Qt4BaseTarget *>(parent)->qt4Project()
        ->hasApplicationProFile(pathFromId(id));
}

bool RemoteLinuxRunConfigurationFactory::canRestore(Target *parent, const QVariantMap &map) const
{
    Q_UNUSED(parent);
    return QString::fromLatin1(ProjectExplorer::idFromMap(map).name()).startsWith(RemoteLinuxRunConfiguration::Id);
}

bool RemoteLinuxRunConfigurationFactory::canClone(Target *parent, RunConfiguration *source) const
{
    const RemoteLinuxRunConfiguration * const rlrc
            = qobject_cast<RemoteLinuxRunConfiguration *>(source);
    const QString idStr = QString::fromLatin1(source->id().name()) + QLatin1Char('.') + rlrc->proFilePath();
    return rlrc && canCreate(parent, Core::Id(idStr.toUtf8().constData()));
}

QList<Core::Id> RemoteLinuxRunConfigurationFactory::availableCreationIds(Target *parent) const
{
    QList<Core::Id> result;
    const QList<DeployConfiguration *> &depConfs = parent->deployConfigurations();
    foreach (const DeployConfiguration * const dc, depConfs) {
        if (dc->id() == RemoteLinuxDeployConfigurationFactory::genericDeployConfigurationId()) {
            QStringList proFiles = qobject_cast<Qt4BaseTarget *>(parent)->qt4Project()
                    ->applicationProFilePathes(RemoteLinuxRunConfiguration::Id);
            foreach (const QString &pf, proFiles)
                result << Core::Id(pf);
            return result;
        }
    }
    return result;
}

QString RemoteLinuxRunConfigurationFactory::displayNameForId(const Core::Id id) const
{
    return QFileInfo(pathFromId(id)).completeBaseName()
        + tr(" (on Remote Generic Linux Host)");
}

RunConfiguration *RemoteLinuxRunConfigurationFactory::create(Target *parent, const Core::Id id)
{
    QTC_ASSERT(canCreate(parent, id), return 0);
    return new RemoteLinuxRunConfiguration(qobject_cast<Qt4BaseTarget *>(parent), id, pathFromId(id));
}

RunConfiguration *RemoteLinuxRunConfigurationFactory::restore(Target *parent,
    const QVariantMap &map)
{
    QTC_ASSERT(canRestore(parent, map), return 0);
    RemoteLinuxRunConfiguration *rc = new RemoteLinuxRunConfiguration(qobject_cast<Qt4BaseTarget *>(parent),
        Core::Id(RemoteLinuxRunConfiguration::Id), QString());
    if (rc->fromMap(map))
        return rc;

    delete rc;
    return 0;
}

RunConfiguration *RemoteLinuxRunConfigurationFactory::clone(Target *parent,
    RunConfiguration *source)
{
    QTC_ASSERT(canClone(parent, source), return 0);
    RemoteLinuxRunConfiguration *old = static_cast<RemoteLinuxRunConfiguration *>(source);
    return new RemoteLinuxRunConfiguration(static_cast<Qt4BaseTarget *>(parent), old);
}

} // namespace Internal
} // namespace RemoteLinux

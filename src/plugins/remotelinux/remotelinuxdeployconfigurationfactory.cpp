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
#include "remotelinuxdeployconfigurationfactory.h"

#include "genericembeddedlinuxtarget.h"
#include "genericdirectuploadstep.h"
#include "remotelinuxdeployconfiguration.h"

#include <QCoreApplication>

using namespace ProjectExplorer;

namespace RemoteLinux {
namespace Internal {
namespace {
QString genericLinuxDisplayName() {
    return QCoreApplication::translate("RemoteLinux", "Deploy to Remote Linux Host");
}
} // anonymous namespace

RemoteLinuxDeployConfigurationFactory::RemoteLinuxDeployConfigurationFactory(QObject *parent)
    : DeployConfigurationFactory(parent)
{ }

QList<Core::Id> RemoteLinuxDeployConfigurationFactory::availableCreationIds(Target *parent) const
{
    QList<Core::Id> ids;
    if (qobject_cast<GenericEmbeddedLinuxTarget *>(parent))
        ids << genericDeployConfigurationId();
    return ids;
}

QString RemoteLinuxDeployConfigurationFactory::displayNameForId(const Core::Id id) const
{
    if (id == genericDeployConfigurationId())
        return genericLinuxDisplayName();
    return QString();
}

bool RemoteLinuxDeployConfigurationFactory::canCreate(Target *parent, const Core::Id id) const
{
    return availableCreationIds(parent).contains(id);
}

DeployConfiguration *RemoteLinuxDeployConfigurationFactory::create(Target *parent,
    const Core::Id id)
{
    Q_ASSERT(canCreate(parent, id));

    DeployConfiguration * const dc = new RemoteLinuxDeployConfiguration(parent, id,
        genericLinuxDisplayName());
    dc->stepList()->insertStep(0, new GenericDirectUploadStep(dc->stepList(),
        GenericDirectUploadStep::stepId()));
    return dc;
}

bool RemoteLinuxDeployConfigurationFactory::canRestore(Target *parent, const QVariantMap &map) const
{
    return canCreate(parent, idFromMap(map));
}

DeployConfiguration *RemoteLinuxDeployConfigurationFactory::restore(Target *parent,
    const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    Core::Id id = idFromMap(map);
    RemoteLinuxDeployConfiguration * const dc = new RemoteLinuxDeployConfiguration(parent, id,
        genericLinuxDisplayName());
    if (!dc->fromMap(map)) {
        delete dc;
        return 0;
    }
    return dc;
}

DeployConfiguration *RemoteLinuxDeployConfigurationFactory::clone(Target *parent,
    DeployConfiguration *product)
{
    if (!canClone(parent, product))
        return 0;
    return new RemoteLinuxDeployConfiguration(parent,
        qobject_cast<RemoteLinuxDeployConfiguration *>(product));
}

Core::Id RemoteLinuxDeployConfigurationFactory::genericDeployConfigurationId()
{
    return Core::Id("DeployToGenericLinux");
}

} // namespace Internal
} // namespace RemoteLinux

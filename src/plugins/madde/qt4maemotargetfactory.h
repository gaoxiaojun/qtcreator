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

#ifndef QT4MAEMOTARGETFACTORY_H
#define QT4MAEMOTARGETFACTORY_H

#include <qt4projectmanager/qt4basetargetfactory.h>

namespace Madde {
namespace Internal {

class Qt4MaemoTargetFactory : public Qt4ProjectManager::Qt4BaseTargetFactory
{
    Q_OBJECT
public:
    Qt4MaemoTargetFactory(QObject *parent = 0);
    ~Qt4MaemoTargetFactory();

    QList<Core::Id> supportedTargetIds() const;
    QString displayNameForId(const Core::Id id) const;
    QIcon iconForId(const Core::Id id) const;

    bool canCreate(ProjectExplorer::Project *parent, const Core::Id id) const;
    bool canRestore(ProjectExplorer::Project *parent, const QVariantMap &map) const;
    ProjectExplorer::Target *restore(ProjectExplorer::Project *parent, const QVariantMap &map);

    QString shadowBuildDirectory(const QString &profilePath, const Core::Id id, const QString &suffix);
    QString buildNameForId(const Core::Id id) const;

    bool supportsTargetId(const Core::Id id) const;

    ProjectExplorer::Target *create(ProjectExplorer::Project *parent, const Core::Id id);
    ProjectExplorer::Target *create(ProjectExplorer::Project *parent, const Core::Id id,
        const QList<Qt4ProjectManager::BuildConfigurationInfo> &infos);

    QSet<QString> targetFeatures(const Core::Id id) const;
};

} // namespace Internal
} // namespace Madde

#endif // QT4MAEMOTARGETFACTORY_H

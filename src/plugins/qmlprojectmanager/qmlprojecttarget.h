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

#ifndef QMLPROJECTTARGET_H
#define QMLPROJECTTARGET_H

#include <projectexplorer/target.h>

#include <QStringList>
#include <QVariantMap>

namespace QmlProjectManager {
class QmlProject;
class QmlProjectRunConfiguration;

namespace Internal {

class QmlProjectTargetFactory;

class QmlProjectTarget : public ProjectExplorer::Target
{
    Q_OBJECT
    friend class QmlProjectTargetFactory;

public:
    explicit QmlProjectTarget(QmlProject *parent);
    ~QmlProjectTarget();

    ProjectExplorer::BuildConfigWidget *createConfigWidget();

    QmlProject *qmlProject() const;

    ProjectExplorer::IBuildConfigurationFactory *buildConfigurationFactory() const;

protected:
    bool fromMap(const QVariantMap &map);
};

class QmlProjectTargetFactory : public ProjectExplorer::ITargetFactory
{
    Q_OBJECT

public:
    explicit QmlProjectTargetFactory(QObject *parent = 0);
    ~QmlProjectTargetFactory();

    bool supportsTargetId(const Core::Id id) const;
    QList<Core::Id> supportedTargetIds() const;
    QString displayNameForId(const Core::Id id) const;

    bool canCreate(ProjectExplorer::Project *parent, const Core::Id id) const;
    QmlProjectTarget *create(ProjectExplorer::Project *parent, const Core::Id id);
    bool canRestore(ProjectExplorer::Project *parent, const QVariantMap &map) const;
    QmlProjectTarget *restore(ProjectExplorer::Project *parent, const QVariantMap &map);
};

} // namespace Internal
} // namespace QmlProjectManager

#endif // QMLPROJECTTARGET_H

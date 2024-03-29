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

#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include "projectexplorer_export.h"

#include <coreplugin/id.h>

#include <QObject>
#include <QString>
#include <QVariantMap>

namespace ProjectExplorer {

class Project;

class PROJECTEXPLORER_EXPORT ProjectConfiguration : public QObject
{
    Q_OBJECT

public:
    // ctors are protected
    virtual ~ProjectConfiguration();

    Core::Id id() const;
    QString displayName() const;

    bool usesDefaultDisplayName() const;
    void setDisplayName(const QString &name);
    void setDefaultDisplayName(const QString &name);

    // Note: Make sure subclasses call the superclasses' fromMap() method!
    virtual bool fromMap(const QVariantMap &map);

    // Note: Make sure subclasses call the superclasses' toMap() method!
    virtual QVariantMap toMap() const;

signals:
    void displayNameChanged();

protected:
    ProjectConfiguration(QObject *parent, const Core::Id &id);
    ProjectConfiguration(QObject *parent, const ProjectConfiguration *source);

private:
    Core::Id m_id;
    QString m_displayName;
    QString m_defaultDisplayName;
};

// helper functions:
PROJECTEXPLORER_EXPORT Core::Id idFromMap(const QVariantMap &map);
PROJECTEXPLORER_EXPORT QString displayNameFromMap(const QVariantMap &map);

} // namespace ProjectExplorer

Q_DECLARE_METATYPE(ProjectExplorer::ProjectConfiguration *)

#endif // PROJECTCONFIGURATION_H

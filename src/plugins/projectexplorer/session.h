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

#ifndef SESSION_H
#define SESSION_H

#include "projectexplorer_export.h"

#include <utils/persistentsettings.h>

#include <QHash>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QFutureInterface>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QTimer;
QT_END_NAMESPACE

namespace Core {
class IMode;
class IEditor;
class IDocument;
}

namespace ProjectExplorer {

class Project;
class Node;
class SessionNode;
class SessionManager;

class PROJECTEXPLORER_EXPORT SessionManager : public QObject
{
    Q_OBJECT

public:
    explicit SessionManager(QObject *parent = 0);
    ~SessionManager();

    // higher level session management
    QString activeSession() const;
    QString lastSession() const;
    QStringList sessions() const;

    bool createSession(const QString &session);

    bool deleteSession(const QString &session);

    bool cloneSession(const QString &original, const QString &clone);
    bool renameSession(const QString &original, const QString &newName);

    bool loadSession(const QString &session);

    bool save();
    void closeAllProjects();

    void addProject(Project *project);
    void addProjects(const QList<Project*> &projects);
    void removeProject(Project *project);
    void removeProjects(QList<Project *> remove);

    void setStartupProject(Project *startupProject);

    QList<Project *> dependencies(const Project *project) const;
    bool hasDependency(const Project *project, const Project *depProject) const;
    bool canAddDependency(const Project *project, const Project *depProject) const;
    bool addDependency(Project *project, Project *depProject);
    void removeDependency(Project *project, Project *depProject);

    QString sessionNameToFileName(const QString &session) const;
    Project *startupProject() const;

    const QList<Project *> &projects() const;

    bool isDefaultVirgin() const;
    bool isDefaultSession(const QString &session) const;

    // Let other plugins store persistent values within the session file
    void setValue(const QString &name, const QVariant &value);
    QVariant value(const QString &name);

    // NBS rewrite projectOrder (dependency management)
    QList<Project *> projectOrder(Project *project = 0) const;

    SessionNode *sessionNode() const;

    Project *projectForNode(ProjectExplorer::Node *node) const;
    Node *nodeForFile(const QString &fileName, Project *project = 0) const;
    Project *projectForFile(const QString &fileName) const;

    QStringList projectsForSessionName(const QString &session) const;

    void reportProjectLoadingProgress();
signals:
    void projectAdded(ProjectExplorer::Project *project);
    void singleProjectAdded(ProjectExplorer::Project *project);
    void aboutToRemoveProject(ProjectExplorer::Project *project);
    void projectDisplayNameChanged(ProjectExplorer::Project *project);
    void projectRemoved(ProjectExplorer::Project *project);

    void startupProjectChanged(ProjectExplorer::Project *project);

    void aboutToUnloadSession(QString sessionName);
    void aboutToLoadSession(QString sessionName);
    void sessionLoaded(QString sessionName);
    void aboutToSaveSession();
    void dependencyChanged(ProjectExplorer::Project *a, ProjectExplorer::Project *b);

private slots:
    void saveActiveMode(Core::IMode *mode);
    void clearProjectFileCache();
    void configureEditor(Core::IEditor *editor, const QString &fileName);
    void updateWindowTitle();

    void markSessionFileDirty(bool makeDefaultVirginDirty = true);
    void sessionLoadingProgress();

    void projectDisplayNameChanged();

private:
    bool projectContainsFile(Project *p, const QString &fileName) const;
    void restoreValues(const Utils::PersistentSettingsReader &reader);
    void restoreDependencies(const Utils::PersistentSettingsReader &reader);
    void restoreStartupProject(const Utils::PersistentSettingsReader &reader);
    void restoreEditors(const Utils::PersistentSettingsReader &reader);
    void restoreProjects(const QStringList &fileList);
    void askUserAboutFailedProjects();

    bool recursiveDependencyCheck(const QString &newDep, const QString &checkDep) const;
    QStringList dependencies(const QString &proName) const;
    QStringList dependenciesOrder() const;

    SessionNode *m_sessionNode;
    QString m_sessionName;
    bool m_virginSession;

    mutable QStringList m_sessions;

    mutable QHash<Project *, QStringList> m_projectFileCache;

    Project *m_startupProject;
    QList<Project *> m_projects;
    QStringList m_failedProjects;
    QMap<QString, QStringList> m_depMap;
    QMap<QString, QVariant> m_values;
    QFutureInterface<void> m_future;
};

} // namespace ProjectExplorer

#endif // SESSION_H

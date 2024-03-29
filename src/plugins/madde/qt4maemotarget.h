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

#ifndef QT4MAEMOTARGET_H
#define QT4MAEMOTARGET_H

#include <remotelinux/abstractembeddedlinuxtarget.h>

#include <QIODevice>
#include <QSharedPointer>
#include <QIcon>

QT_FORWARD_DECLARE_CLASS(QDateTime)
QT_FORWARD_DECLARE_CLASS(QFile)

namespace Qt4ProjectManager { class Qt4Project; }
namespace RemoteLinux { class DeploymentSettingsAssistant; }
namespace Utils { class FileSystemWatcher; }

namespace Madde {
namespace Internal {
class Qt4MaemoDeployConfigurationFactory;
class WatchableFile;

class AbstractQt4MaemoTarget : public RemoteLinux::AbstractEmbeddedLinuxTarget
{
    friend class Qt4MaemoTargetFactory;
    Q_OBJECT
public:
    AbstractQt4MaemoTarget(Qt4ProjectManager::Qt4Project *parent, const Core::Id id,
                           const QString &qmakeScope);
    virtual ~AbstractQt4MaemoTarget();

    void createApplicationProFiles(bool reparse);
    QList<ProjectExplorer::RunConfiguration *> runConfigurationsForNode(ProjectExplorer::Node *n);
    QList<ProjectExplorer::ToolChain *> possibleToolChains(ProjectExplorer::BuildConfiguration *bc) const;

    virtual bool allowsRemoteMounts() const = 0;
    virtual bool allowsPackagingDisabling() const = 0;
    virtual bool allowsQmlDebugging() const = 0;

    virtual QString projectVersion(QString *error = 0) const = 0;
    virtual QString packageName() const = 0;
    virtual QString shortDescription() const = 0;
    virtual QString packageFileName() const = 0;

    bool setProjectVersion(const QString &version, QString *error = 0);
    bool setPackageName(const QString &packageName);
    bool setShortDescription(const QString &description);

    RemoteLinux::DeploymentSettingsAssistant *deploymentSettingsAssistant() const {
        return m_deploymentSettingsAssistant;
    }

protected:
    enum ActionStatus { NoActionRequired, ActionSuccessful, ActionFailed };

    void raiseError(const QString &reason);
    QSharedPointer<QFile> openFile(const QString &filePath,
        QIODevice::OpenMode mode, QString *error) const;

    Utils::FileSystemWatcher* const m_filesWatcher;

private slots:
    void handleTargetAdded(ProjectExplorer::Target *target);
    void handleFromMapFinished();
    void handleTargetToBeRemoved(ProjectExplorer::Target *target);

private:
    virtual bool setProjectVersionInternal(const QString &version,
        QString *error = 0) = 0;
    virtual bool setPackageNameInternal(const QString &packageName) = 0;
    virtual bool setShortDescriptionInternal(const QString &description) = 0;
    virtual ActionStatus createSpecialTemplates() = 0;
    virtual void handleTargetAddedSpecial() = 0;
    virtual bool targetCanBeRemoved() const = 0;
    virtual void removeTarget() = 0;
    virtual QStringList packagingFilePaths() const = 0;

    ActionStatus createTemplates();
    bool initPackagingSettingsFromOtherTarget();
    virtual bool initAdditionalPackagingSettingsFromOtherTarget() = 0;

    RemoteLinux::DeploymentSettingsAssistant * const m_deploymentSettingsAssistant;
    bool m_isInitialized;
};


class AbstractDebBasedQt4MaemoTarget : public AbstractQt4MaemoTarget
{
    Q_OBJECT
public:
    AbstractDebBasedQt4MaemoTarget(Qt4ProjectManager::Qt4Project *parent, const Core::Id id,
                                   const QString &qmakeScope);
    ~AbstractDebBasedQt4MaemoTarget();

    QString debianDirPath() const;
    QStringList debianFiles() const;

    virtual QString debianDirName() const = 0;
    virtual QString projectVersion(QString *error = 0) const;
    virtual QString packageName() const;
    virtual QString shortDescription() const;
    virtual QString packageFileName() const;

    bool setPackageManagerIcon(const QString &iconFilePath, QString *error = 0);
    QIcon packageManagerIcon(QString *error = 0) const;
    bool setPackageManagerName(const QString &name, QString *error = 0);
    QString packageManagerName() const;

    virtual QSize packageManagerIconSize() const = 0;

signals:
    void debianDirContentsChanged();
    void changeLogChanged();
    void controlChanged();

protected:
    virtual void handleTargetAddedSpecial();
    bool adaptControlFileField(QByteArray &document, const QByteArray &fieldName,
        const QByteArray &newFieldValue);

private slots:
    void handleDebianDirContentsChanged();

private:
    virtual bool setProjectVersionInternal(const QString &version,
        QString *error = 0);
    virtual bool setPackageNameInternal(const QString &packageName);
    virtual bool setShortDescriptionInternal(const QString &description);

    virtual ActionStatus createSpecialTemplates();
    virtual bool targetCanBeRemoved() const;
    virtual void removeTarget();
    virtual bool initAdditionalPackagingSettingsFromOtherTarget();
    virtual QStringList packagingFilePaths() const;

    virtual void addAdditionalControlFileFields(QByteArray &controlContents) = 0;
    virtual QByteArray packageManagerNameFieldName() const = 0;
    virtual QByteArray defaultSection() const = 0;

    QString changeLogFilePath() const;
    QString controlFilePath() const;
    QString rulesFilePath() const;
    QByteArray controlFileFieldValue(const QString &key, bool multiLine) const;
    bool setControlFieldValue(const QByteArray &fieldName,
        const QByteArray &fieldValue);
    bool adaptRulesFile();
    bool adaptControlFile();
    bool setPackageManagerIconInternal(const QString &iconFilePath,
        QString *error = 0);
    QString defaultPackageFileName() const;
    bool setPackageManagerNameInternal(const QString &name, QString *error = 0);
    QString shortMonthName(const QDateTime &dt) const;
    QString shortDayOfWeekName(const QDateTime &dt) const;

    WatchableFile *m_controlFile;
    WatchableFile *m_changeLogFile;
};


class AbstractRpmBasedQt4MaemoTarget : public AbstractQt4MaemoTarget
{
    Q_OBJECT
public:
    AbstractRpmBasedQt4MaemoTarget(Qt4ProjectManager::Qt4Project *parent, const Core::Id id,
                                   const QString &qmakeScope);
    ~AbstractRpmBasedQt4MaemoTarget();

    virtual bool allowsRemoteMounts() const { return false; }
    virtual bool allowsPackagingDisabling() const { return false; }
    virtual bool allowsQmlDebugging() const { return false; }

    virtual QString projectVersion(QString *error = 0) const;
    virtual QString packageName() const;
    virtual QString shortDescription() const;
    virtual QString packageFileName() const;

    QString specFilePath() const;

signals:
    void specFileChanged();

private:
    virtual bool setProjectVersionInternal(const QString &version,
        QString *error = 0);
    virtual bool setPackageNameInternal(const QString &packageName);
    virtual bool setShortDescriptionInternal(const QString &description);
    virtual ActionStatus createSpecialTemplates();
    virtual void handleTargetAddedSpecial();
    virtual bool targetCanBeRemoved() const;
    virtual void removeTarget();
    virtual bool initAdditionalPackagingSettingsFromOtherTarget();
    virtual QStringList packagingFilePaths() const { return QStringList(specFilePath()); }

    virtual QString specFileName() const = 0;

    QByteArray getValueForTag(const QByteArray &tag, QString *error) const;
    bool setValueForTag(const QByteArray &tag, const QByteArray &value,
        QString *error);

    WatchableFile *m_specFile;
};


class Qt4Maemo5Target : public AbstractDebBasedQt4MaemoTarget
{
    Q_OBJECT
public:
    Qt4Maemo5Target(Qt4ProjectManager::Qt4Project *parent, const Core::Id id);
    virtual ~Qt4Maemo5Target();

    virtual bool supportsDevice(const QSharedPointer<const ProjectExplorer::IDevice> &device) const;

    virtual bool allowsRemoteMounts() const { return true; }
    virtual bool allowsPackagingDisabling() const { return true; }
    virtual bool allowsQmlDebugging() const { return false; }

    virtual QSize packageManagerIconSize() const;

    static QString defaultDisplayName();

private:
    virtual void addAdditionalControlFileFields(QByteArray &controlContents);
    virtual QString debianDirName() const;
    virtual QByteArray packageManagerNameFieldName() const;
    virtual QByteArray defaultSection() const;
};


class Qt4HarmattanTarget : public AbstractDebBasedQt4MaemoTarget
{
    Q_OBJECT
public:
    Qt4HarmattanTarget(Qt4ProjectManager::Qt4Project *parent, const Core::Id id);
    virtual ~Qt4HarmattanTarget();

    virtual bool supportsDevice(const QSharedPointer<const ProjectExplorer::IDevice> &device) const;

    virtual bool allowsRemoteMounts() const { return false; }
    virtual bool allowsPackagingDisabling() const { return false; }
    virtual bool allowsQmlDebugging() const { return true; }

    virtual QSize packageManagerIconSize() const;

    static QString defaultDisplayName();
    static QString aegisManifestFileName();

private:
    void handleTargetAddedSpecial();
    virtual void addAdditionalControlFileFields(QByteArray &controlContents);
    virtual QString debianDirName() const;
    virtual QByteArray packageManagerNameFieldName() const;
    virtual QByteArray defaultSection() const;
};


class Qt4MeegoTarget : public AbstractRpmBasedQt4MaemoTarget
{
    Q_OBJECT
public:
    Qt4MeegoTarget(Qt4ProjectManager::Qt4Project *parent, const Core::Id id);
    virtual ~Qt4MeegoTarget();
    virtual bool supportsDevice(const QSharedPointer<const ProjectExplorer::IDevice> &device) const;
    static QString defaultDisplayName();
private:
    virtual QString specFileName() const;
};

} // namespace Internal
} // namespace Madde

#endif // QT4MAEMOTARGET_H

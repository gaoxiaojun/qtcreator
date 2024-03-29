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

#ifndef SYMBIANQTVERSION_H
#define SYMBIANQTVERSION_H

#include <qtsupport/baseqtversion.h>

namespace Qt4ProjectManager {
namespace Internal {

class SymbianQtVersion : public QtSupport::BaseQtVersion
{
public:
    SymbianQtVersion();
    SymbianQtVersion(const Utils::FileName &path, bool isAutodetected = false, const QString &autodetectionSource = QString());
    SymbianQtVersion *clone() const;
    ~SymbianQtVersion();

    bool equals(BaseQtVersion *other);

    QString type() const;

    bool isValid() const;
    QString invalidReason() const;

    bool toolChainAvailable(const QString &id) const;

    void restoreLegacySettings(QSettings *s);
    void fromMap(const QVariantMap &map);
    QVariantMap toMap() const;

    QList<ProjectExplorer::Abi> detectQtAbis() const;

    bool supportsTargetId(const Core::Id id) const;
    QSet<Core::Id> supportedTargetIds() const;

    QString description() const;

    bool supportsShadowBuilds() const;
    bool supportsBinaryDebuggingHelper() const;
    void addToEnvironment(Utils::Environment &env) const;
    QList<ProjectExplorer::HeaderPath> systemHeaderPathes() const;

    ProjectExplorer::IOutputParser *createOutputParser() const;

    QString systemRoot() const;
    void setSystemRoot(const QString &);

    bool isBuildWithSymbianSbsV2() const;

    QString sbsV2Directory() const;
    void setSbsV2Directory(const QString &directory);

    QtSupport::QtConfigWidget *createConfigurationWidget() const;

    Core::FeatureSet availableFeatures() const;
    QString platformName() const;
    QString platformDisplayName() const;

protected:
    QList<ProjectExplorer::Task> reportIssuesImpl(const QString &proFile, const QString &buildDir);
    void parseMkSpec(ProFileEvaluator *) const;
private:
    QString m_sbsV2Directory;
    QString m_systemRoot;
    mutable bool m_validSystemRoot;
    mutable bool m_isBuildUsingSbsV2;
};

class SymbianQtConfigWidget : public QtSupport::QtConfigWidget
{
    Q_OBJECT
public:
    SymbianQtConfigWidget(SymbianQtVersion *version);
public slots:
    void updateCurrentSbsV2Directory(const QString &path);
    void updateCurrentS60SDKDirectory(const QString &path);
private:
    SymbianQtVersion *m_version;
};

}
}

#endif // SYMBIANQTVERSION_H

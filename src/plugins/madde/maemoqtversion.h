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
#ifndef MAEMOQTVERSION_H
#define MAEMOQTVERSION_H

#include <qtsupport/baseqtversion.h>

namespace Madde {
namespace Internal {

class MaemoQtVersion : public QtSupport::BaseQtVersion
{
public:
    MaemoQtVersion();
    MaemoQtVersion(const Utils::FileName &path, bool isAutodetected = false, const QString &autodetectionSource = QString());
    ~MaemoQtVersion();

    void fromMap(const QVariantMap &map);
    MaemoQtVersion *clone() const;

    QString type() const;
    bool isValid() const;
    QString systemRoot() const;
    QList<ProjectExplorer::Abi> detectQtAbis() const;
    void addToEnvironment(Utils::Environment &env) const;

    bool supportsTargetId(const Core::Id id) const;
    QSet<Core::Id> supportedTargetIds() const;

    QString description() const;

    bool supportsShadowBuilds() const;
    QString deviceType() const;
    Core::FeatureSet availableFeatures() const;
    QString platformName() const;
    QString platformDisplayName() const;

private:
    mutable QString m_systemRoot;
    mutable QString m_deviceType;
    mutable bool m_isvalidVersion;
    mutable bool m_initialized;
};

} // namespace Internal
} // namespace Madde

#endif // MAEMOQTVERSION_H

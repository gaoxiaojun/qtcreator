/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
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
#ifndef ABSTRACTPACKAGINGSTEP_H
#define ABSTRACTPACKAGINGSTEP_H

#include "remotelinux_export.h"

#include <projectexplorer/buildstep.h>

namespace RemoteLinux {
class RemoteLinuxDeployConfiguration;
class DeploymentInfo;

namespace Internal {
class AbstractPackagingStepPrivate;
}

class REMOTELINUX_EXPORT AbstractPackagingStep : public ProjectExplorer::BuildStep
{
    Q_OBJECT

public:
    AbstractPackagingStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id);
    AbstractPackagingStep(ProjectExplorer::BuildStepList *bsl, AbstractPackagingStep *other);
    ~AbstractPackagingStep();

    QString packageFilePath() const;
    QString cachedPackageFilePath() const;
    bool init();

signals:
    void packageFilePathChanged();
    void unmodifyDeploymentInfo();

protected:
    void setPackagingStarted();
    void setPackagingFinished(bool success);

    void raiseError(const QString &errorMessage);
    RemoteLinuxDeployConfiguration *deployConfiguration() const;
    QString cachedPackageDirectory() const;
    QString packageDirectory() const;

    virtual bool isPackagingNeeded() const;

private slots:
    void handleBuildConfigurationChanged();
    void setDeploymentInfoUnmodified();

private:
    virtual QString packageFileName() const = 0;

    void ctor();

    Internal::AbstractPackagingStepPrivate *d;
};

} // namespace RemoteLinux

#endif // ABSTRACTPACKAGINGSTEP_H

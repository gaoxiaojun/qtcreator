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

#ifndef MAEMOCONSTANTS_H
#define MAEMOCONSTANTS_H

#include <QLatin1String>

namespace Madde {
namespace Internal {

const char Maemo5OsType[] = "Maemo5OsType";
const char HarmattanOsType[] = "HarmattanOsType";
const char MeeGoOsType[] = "MeeGoOsType";

#define PREFIX "Qt4ProjectManager.MaemoRunConfiguration"

#ifdef Q_OS_WIN32
#define EXEC_SUFFIX ".exe"
#else
#define EXEC_SUFFIX ""
#endif

static const char MAEMO_RC_ID[] = PREFIX;
static const char MAEMO_RC_ID_PREFIX[] = PREFIX ".";

static const QLatin1String LastDeployedHostsKey(PREFIX ".LastDeployedHosts");
static const QLatin1String LastDeployedFilesKey(PREFIX ".LastDeployedFiles");
static const QLatin1String LastDeployedRemotePathsKey(PREFIX ".LastDeployedRemotePaths");
static const QLatin1String LastDeployedTimesKey(PREFIX ".LastDeployedTimes");
static const QLatin1String ExportedLocalDirsKey(PREFIX ".ExportedLocalDirs");
static const QLatin1String RemoteMountPointsKey(PREFIX ".RemoteMountPoints");

} // namespace Internal
} // namespace Madde

#endif  // MAEMOCONSTANTS_H

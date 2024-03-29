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
#ifndef QMLJSINSPECTORPLUGIN_H
#define QMLJSINSPECTORPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <qmljs/qmljsmodelmanagerinterface.h>

#include <QObject>
#include <QPointer>
#include <QTimer>

namespace Core {
class IMode;
} // namespace Core

namespace QmlJSInspector {
namespace Internal {

class ClientProxy;
class InspectorUi;

class InspectorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin"
                      FILE "QmlJSInspector.json")

public:
    InspectorPlugin();
    virtual ~InspectorPlugin();

    //static InspectorPlugin *instance();

    QmlJS::ModelManagerInterface *modelManager() const;
    InspectorUi *inspector() const;

    // ExtensionSystem::IPlugin interface
    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();
    virtual ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();

private slots:
    void objectAdded(QObject *object);
    void disconnect();
    void clientProxyConnected();
    void modeAboutToChange(Core::IMode *mode);

private:
    void createActions();

private:
    ClientProxy *m_clientProxy;
    InspectorUi *m_inspectorUi;
};

} // namespace Internal
} // namespace QmlJSInspector

#endif // QMLINSPECTORPLUGIN_H

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

#ifndef QMLPROFILERCLIENTMANAGER_H
#define QMLPROFILERCLIENTMANAGER_H

#include <QObject>
#include <QStringList>

#include "qmlprofilerstatemanager.h"
#include <qmldebug/qmlprofilereventlocation.h>

namespace QmlProfiler {
namespace Internal {

class QmlProfilerClientManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlProfilerClientManager(QObject *parent = 0);
    ~QmlProfilerClientManager();

    void registerProfilerStateManager(QmlProfilerStateManager *profilerState);

    void setTcpConnection(QString host, quint64 port);
    void setOstConnection(QString ostDevice);

    void clearBufferedData();
    void discardPendingData();

signals:
    void connectionFailed();

    // data
    void addRangedEvent(int,qint64,qint64,QStringList,QmlDebug::QmlEventLocation);
    void addV8Event(int,QString,QString,int,double,double);
    void addFrameEvent(qint64,int,int);
    void traceStarted(qint64);
    void traceFinished(qint64);
    void dataReadyForProcessing();

public slots:
    void connectClient(quint16 port);
    void disconnectClient();

private slots:
    void tryToConnect();
    void connectionStateChanged();
    void retryMessageBoxFinished(int result);

    void qmlComplete();
    void v8Complete();

    void profilerStateChanged();
    void clientRecordingChanged();
    void serverRecordingChanged();

private:
    class QmlProfilerClientManagerPrivate;
    QmlProfilerClientManagerPrivate *d;

    void connectToClient();

    void enableServices();
    void connectClientSignals();
    void disconnectClientSignals();

    void stopClientsRecording();
};

}
}

#endif // QMLPROFILERCLIENTMANAGER_H

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

#include "qv8profilerclient.h"

namespace QmlDebug {

class QV8ProfilerClientPrivate {
public:
    QV8ProfilerClientPrivate(QV8ProfilerClient *_q)
        : q(_q)
        , recording(false)
    {
    }

    void sendRecordingStatus();

    QV8ProfilerClient *q;
    bool recording;
};

} // namespace QmlDebug

using namespace QmlDebug;

void QV8ProfilerClientPrivate::sendRecordingStatus()
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    QByteArray cmd("V8PROFILER");
    QByteArray option("");
    QByteArray title("");

    if (recording) {
        option = "start";
    } else {
        option = "stop";
    }
    stream << cmd << option << title;
    q->sendMessage(ba);
}

QV8ProfilerClient::QV8ProfilerClient(QmlDebugConnection *client)
    : QmlDebugClient(QLatin1String("V8Profiler"), client)
    , d(new QV8ProfilerClientPrivate(this))
{
}

QV8ProfilerClient::~QV8ProfilerClient()
{
    //Disable profiling if started by client
    //Profiling data will be lost!!
    if (isRecording())
        setRecording(false);
    delete d;
}

void QV8ProfilerClient::clearData()
{
    emit cleared();
}

bool QV8ProfilerClient::isEnabled() const
{
    return status() == Enabled;
}

void QV8ProfilerClient::sendRecordingStatus()
{
    d->sendRecordingStatus();
}

bool QV8ProfilerClient::isRecording() const
{
    return d->recording;
}

void QV8ProfilerClient::setRecording(bool v)
{
    if (v == d->recording)
        return;

    d->recording = v;

    if (status() == Enabled) {
        sendRecordingStatus();
    }

    emit recordingChanged(v);
}

void QV8ProfilerClient::setRecordingFromServer(bool v)
{
    if (v == d->recording)
        return;

    d->recording = v;

    emit recordingChanged(v);
}

void QV8ProfilerClient::statusChanged(Status /*status*/)
{
    emit enabledChanged();
}

void QV8ProfilerClient::messageReceived(const QByteArray &data)
{
    QByteArray rwData = data;
    QDataStream stream(&rwData, QIODevice::ReadOnly);

    int messageType;

    stream >> messageType;

    if (messageType == V8Complete) {
        setRecordingFromServer(false);
        emit complete();
    } else if (messageType == V8ProfilingStarted) {
        setRecordingFromServer(true);
    } else if (messageType == V8Entry) {
        QString filename;
        QString function;
        int lineNumber;
        double totalTime;
        double selfTime;
        int depth;

        stream  >> filename >> function >> lineNumber >> totalTime >> selfTime >> depth;
        emit this->v8range(depth, function, filename, lineNumber, totalTime, selfTime);
    }
}


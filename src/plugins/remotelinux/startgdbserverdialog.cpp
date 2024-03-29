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

#include "startgdbserverdialog.h"

#include "remotelinuxprocesslist.h"
#include "linuxdeviceconfiguration.h"
#include "remotelinuxusedportsgatherer.h"

#include <coreplugin/icore.h>
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/devicesupport/devicemanagermodel.h>
#include <utils/pathchooser.h>
#include <utils/portlist.h>
#include <utils/qtcassert.h>
#include <utils/ssh/sshconnection.h>
#include <utils/ssh/sshremoteprocessrunner.h>

#include <QVariant>
#include <QSettings>

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSpacerItem>
#include <QTableView>
#include <QTextBrowser>
#include <QVBoxLayout>

using namespace Core;
using namespace ProjectExplorer;
using namespace Utils;

const char LastSysroot[] = "RemoteLinux/LastSysroot";
const char LastDevice[] = "RemoteLinux/LastDevice";
const char LastProcessName[] = "RemoteLinux/LastProcessName";
//const char LastLocalExecutable[] = "RemoteLinux/LastLocalExecutable";

namespace RemoteLinux {
namespace Internal {

class StartGdbServerDialogPrivate
{
public:
    StartGdbServerDialogPrivate(StartGdbServerDialog *q);

    LinuxDeviceConfiguration::ConstPtr currentDevice() const
    {
        DeviceManager *devices = DeviceManager::instance();
        return devices->deviceAt(deviceComboBox->currentIndex())
            .dynamicCast<const LinuxDeviceConfiguration>();
    }

    StartGdbServerDialog *q;
    bool startServerOnly;
    AbstractRemoteLinuxProcessList *processList;
    QSortFilterProxyModel proxyModel;

    QComboBox *deviceComboBox;
    QLineEdit *processFilterLineEdit;
    QTableView *tableView;
    QPushButton *attachProcessButton;
    QTextBrowser *textBrowser;
    QPushButton *closeButton;
    PathChooser *sysrootPathChooser;

    RemoteLinuxUsedPortsGatherer gatherer;
    SshRemoteProcessRunner runner;
    QSettings *settings;
    QString remoteCommandLine;
};

StartGdbServerDialogPrivate::StartGdbServerDialogPrivate(StartGdbServerDialog *q)
    : q(q), startServerOnly(true), processList(0)
{
    settings = ICore::settings();

    deviceComboBox = new QComboBox(q);

    sysrootPathChooser = new PathChooser(q);
    sysrootPathChooser->setExpectedKind(PathChooser::Directory);
    sysrootPathChooser->setPromptDialogTitle(StartGdbServerDialog::tr("Select Sysroot"));
    sysrootPathChooser->setPath(settings->value(LastSysroot).toString());

    //executablePathChooser = new PathChooser(q);
    //executablePathChooser->setExpectedKind(PathChooser::File);
    //executablePathChooser->setPromptDialogTitle(StartGdbServerDialog::tr("Select Executable"));
    //executablePathChooser->setPath(settings->value(LastLocalExecutable).toString());

    processFilterLineEdit = new QLineEdit(q);
    processFilterLineEdit->setText(settings->value(LastProcessName).toString());
    processFilterLineEdit->selectAll();

    tableView = new QTableView(q);
    tableView->setShowGrid(false);
    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setDefaultSectionSize(100);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    attachProcessButton = new QPushButton(q);
    attachProcessButton->setText(StartGdbServerDialog::tr("&Attach to Selected Process"));

    closeButton = new QPushButton(q);
    closeButton->setText(StartGdbServerDialog::tr("Close"));

    textBrowser = new QTextBrowser(q);
    textBrowser->setEnabled(false);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(StartGdbServerDialog::tr("Device:"), deviceComboBox);
    formLayout->addRow(StartGdbServerDialog::tr("Sysroot:"), sysrootPathChooser);
    formLayout->addRow(StartGdbServerDialog::tr("&Filter entries:"), processFilterLineEdit);

    QHBoxLayout *horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->addStretch(1);
    horizontalLayout2->addWidget(attachProcessButton);
    horizontalLayout2->addWidget(closeButton);

    formLayout->addRow(tableView);
    formLayout->addRow(textBrowser);
    formLayout->addRow(horizontalLayout2);
    q->setLayout(formLayout);
}

} // namespace Internal


StartGdbServerDialog::StartGdbServerDialog(QWidget *parent) :
    QDialog(parent),
    d(new Internal::StartGdbServerDialogPrivate(this))
{
    setWindowTitle(tr("List of Remote Processes"));

    DeviceManager *devices = DeviceManager::instance();
    DeviceManagerModel * const model = new DeviceManagerModel(devices, this);

    QObject::connect(d->closeButton, SIGNAL(clicked()), this, SLOT(reject()));

    d->deviceComboBox->setModel(model);
    d->deviceComboBox->setCurrentIndex(d->settings->value(LastDevice).toInt());
    connect(&d->gatherer, SIGNAL(error(QString)), SLOT(portGathererError(QString)));
    connect(&d->gatherer, SIGNAL(portListReady()), SLOT(portListReady()));
    if (devices->deviceCount() == 0) {
        d->tableView->setEnabled(false);
    } else {
        d->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        d->proxyModel.setDynamicSortFilter(true);
        d->proxyModel.setFilterKeyColumn(-1);
        d->tableView->setModel(&d->proxyModel);
        connect(d->processFilterLineEdit, SIGNAL(textChanged(QString)),
            &d->proxyModel, SLOT(setFilterRegExp(QString)));

        connect(d->tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateButtons()));
        connect(d->sysrootPathChooser, SIGNAL(changed(QString)),
                SLOT(updateButtons()));
        //connect(d->updateListButton, SIGNAL(clicked()),
        //    SLOT(updateProcessList()));
        connect(d->attachProcessButton, SIGNAL(clicked()), SLOT(attachToProcess()));
        connect(&d->proxyModel, SIGNAL(layoutChanged()),
            SLOT(handleProcessListUpdated()));
        connect(d->deviceComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(attachToDevice(int)));
        updateButtons();
        attachToDevice(d->deviceComboBox->currentIndex());
    }
}

StartGdbServerDialog::~StartGdbServerDialog()
{
    delete d->processList;
    delete d;
}

void StartGdbServerDialog::attachToDevice(int index)
{
    DeviceManager *devices = DeviceManager::instance();
    LinuxDeviceConfiguration::ConstPtr device
        = devices->deviceAt(index).dynamicCast<const LinuxDeviceConfiguration>();
    if (!device)
        return;
    delete d->processList;
    d->processList = new GenericRemoteLinuxProcessList(device);
    d->proxyModel.setSourceModel(d->processList);
    connect(d->processList, SIGNAL(error(QString)),
        SLOT(handleRemoteError(QString)));
    connect(d->processList, SIGNAL(modelReset()),
        SLOT(handleProcessListUpdated()));
    connect(d->processList, SIGNAL(processKilled()),
        SLOT(handleProcessKilled()), Qt::QueuedConnection);
    updateProcessList();
}

void StartGdbServerDialog::handleRemoteError(const QString &errorMsg)
{
    QMessageBox::critical(this, tr("Remote Error"), errorMsg);
    updateButtons();
}

void StartGdbServerDialog::handleProcessListUpdated()
{
    d->tableView->resizeRowsToContents();
    updateButtons();
}

void StartGdbServerDialog::updateProcessList()
{
    d->attachProcessButton->setEnabled(false);
    d->processList->update();
    d->proxyModel.setFilterRegExp(QString());
    d->proxyModel.setFilterRegExp(d->processFilterLineEdit->text());
    updateButtons();
}

void StartGdbServerDialog::attachToProcess()
{
    const QModelIndexList &indexes =
            d->tableView->selectionModel()->selectedIndexes();
    if (indexes.empty())
        return;
    d->attachProcessButton->setEnabled(false);

    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    if (!device)
        return;
    PortList ports = device->freePorts();
    const int port = d->gatherer.getNextFreePort(&ports);
    const int row = d->proxyModel.mapToSource(indexes.first()).row();
    QTC_ASSERT(row >= 0, return);
    const int pid = d->processList->pidAt(row);
    d->remoteCommandLine = d->processList->commandLineAt(row);
    if (port == -1) {
        reportFailure();
        return;
    }

    d->settings->setValue(LastSysroot, d->sysrootPathChooser->path());
    d->settings->setValue(LastDevice, d->deviceComboBox->currentIndex());
    d->settings->setValue(LastProcessName, d->processFilterLineEdit->text());

    startGdbServerOnPort(port, pid);
}

void StartGdbServerDialog::reportFailure()
{
    QTC_ASSERT(false, /**/);
    logMessage(tr("Process aborted"));
}

void StartGdbServerDialog::logMessage(const QString &line)
{
    d->textBrowser->append(line);
}

void StartGdbServerDialog::handleProcessKilled()
{
    updateProcessList();
}

void StartGdbServerDialog::updateButtons()
{
    d->attachProcessButton->setEnabled(d->tableView->selectionModel()->hasSelection()
        || d->proxyModel.rowCount() == 1);
}

void StartGdbServerDialog::portGathererError(const QString &text)
{
    logMessage(tr("Could not retrieve list of free ports:"));
    logMessage(text);
    reportFailure();
}

void StartGdbServerDialog::portListReady()
{
    updateButtons();
}

void StartGdbServerDialog::startGdbServer()
{
    d->startServerOnly = true;
    if (exec() == QDialog::Rejected)
        return;
    d->gatherer.start(d->currentDevice());
}

void StartGdbServerDialog::attachToRemoteProcess()
{
    d->startServerOnly = false;
    if (exec() == QDialog::Rejected)
        return;
    d->gatherer.start(d->currentDevice());
}

void StartGdbServerDialog::handleConnectionError()
{
    logMessage(tr("Connection error: %1").arg(d->runner.lastConnectionErrorString()));
    emit processAborted();
}

void StartGdbServerDialog::handleProcessStarted()
{
    logMessage(tr("Starting gdbserver..."));
}

void StartGdbServerDialog::handleProcessOutputAvailable(const QByteArray &ba)
{
    logMessage(QString::fromUtf8(ba.trimmed()));
}

void StartGdbServerDialog::handleProcessErrorOutput(const QByteArray &ba)
{
    logMessage(QString::fromUtf8(ba.trimmed()));
    // "Attached; pid = 16740"
    // "Listening on port 10000"
    foreach (const QByteArray &line, ba.split('\n')) {
        if (line.startsWith("Listening on port")) {
            const int port = line.mid(18).trimmed().toInt();
            reportOpenPort(port);
        }
    }
}

void StartGdbServerDialog::reportOpenPort(int port)
{
    logMessage(tr("Port %1 is now accessible.").arg(port));
    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    QString channel = QString("%1:%2").arg(device->sshParameters().host).arg(port);
    logMessage(tr("Server started on %1").arg(channel));

    const char *member = d->startServerOnly ? "gdbServerStarted" : "attachedToProcess";
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    QObject *ob = pm->getObjectByName("DebuggerCore");
    if (ob) {
        QMetaObject::invokeMethod(ob, member, Qt::QueuedConnection,
            Q_ARG(QString, channel),
            Q_ARG(QString, d->sysrootPathChooser->path()),
            Q_ARG(QString, d->remoteCommandLine));
    }
    close();
}

void StartGdbServerDialog::handleProcessClosed(int status)
{
    logMessage(tr("Process gdbserver finished. Status: %1").arg(status));
}

void StartGdbServerDialog::startGdbServerOnPort(int port, int pid)
{
    LinuxDeviceConfiguration::ConstPtr device = d->currentDevice();
    connect(&d->runner, SIGNAL(connectionError()), SLOT(handleConnectionError()));
    connect(&d->runner, SIGNAL(processStarted()), SLOT(handleProcessStarted()));
    connect(&d->runner, SIGNAL(processOutputAvailable(QByteArray)),
        SLOT(handleProcessOutputAvailable(QByteArray)));
    connect(&d->runner, SIGNAL(processErrorOutputAvailable(QByteArray)),
        SLOT(handleProcessErrorOutput(QByteArray)));
    connect(&d->runner, SIGNAL(processClosed(int)), SLOT(handleProcessClosed(int)));

    QByteArray cmd = "/usr/bin/gdbserver --attach :"
        + QByteArray::number(port) + " " + QByteArray::number(pid);
    logMessage(tr("Running command: %1").arg(QString::fromLatin1(cmd)));
    d->runner.run(cmd, device->sshParameters());
}

} // namespace RemoteLinux

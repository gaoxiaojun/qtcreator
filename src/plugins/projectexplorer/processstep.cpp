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

#include "processstep.h"
#include "buildstep.h"
#include "project.h"
#include "buildconfiguration.h"
#include "projectexplorerconstants.h"
#include "target.h"

#include <coreplugin/idocument.h>
#include <utils/qtcprocess.h>
#include <utils/qtcassert.h>

#include <QDebug>
#include <QFileDialog>

using namespace ProjectExplorer;
using namespace ProjectExplorer::Internal;

namespace {
const char PROCESS_STEP_ID[] = "ProjectExplorer.ProcessStep";
const char PROCESS_COMMAND_KEY[] = "ProjectExplorer.ProcessStep.Command";
const char PROCESS_WORKINGDIRECTORY_KEY[] = "ProjectExplorer.ProcessStep.WorkingDirectory";
const char PROCESS_ARGUMENTS_KEY[] = "ProjectExplorer.ProcessStep.Arguments";
}

ProcessStep::ProcessStep(BuildStepList *bsl) :
    AbstractProcessStep(bsl, Core::Id(PROCESS_STEP_ID))
{
    ctor();
}

ProcessStep::ProcessStep(BuildStepList *bsl, const Core::Id id) :
    AbstractProcessStep(bsl, id)
{
    ctor();
}

ProcessStep::ProcessStep(BuildStepList *bsl, ProcessStep *bs) :
    AbstractProcessStep(bsl, bs),
    m_command(bs->m_command),
    m_arguments(bs->m_arguments),
    m_workingDirectory(bs->m_workingDirectory)
{
    ctor();
}

void ProcessStep::ctor()
{
    //: Default ProcessStep display name
    setDefaultDisplayName(tr("Custom Process Step"));
    if (m_workingDirectory.isEmpty())
        m_workingDirectory = QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR);
}

ProcessStep::~ProcessStep()
{
}

bool ProcessStep::init()
{
    BuildConfiguration *bc = buildConfiguration();
    if (!bc)
        bc = target()->activeBuildConfiguration();
    ProcessParameters *pp = processParameters();
    pp->setMacroExpander(bc->macroExpander());
    pp->setEnvironment(bc->environment());
    pp->setWorkingDirectory(workingDirectory());
    pp->setCommand(m_command);
    pp->setArguments(m_arguments);
    setOutputParser(bc->createOutputParser());
    return AbstractProcessStep::init();
}

void ProcessStep::run(QFutureInterface<bool> & fi)
{
    return AbstractProcessStep::run(fi);
}

BuildStepConfigWidget *ProcessStep::createConfigWidget()
{
    return new ProcessStepConfigWidget(this);
}

bool ProcessStep::immutable() const
{
    return false;
}

QString ProcessStep::command() const
{
    return m_command;
}

QString ProcessStep::arguments() const
{
    return m_arguments;
}

QString ProcessStep::workingDirectory() const
{
    return m_workingDirectory;
}

void ProcessStep::setCommand(const QString &command)
{
    m_command = command;
}

void ProcessStep::setArguments(const QString &arguments)
{
    m_arguments = arguments;
}

void ProcessStep::setWorkingDirectory(const QString &workingDirectory)
{
    if (workingDirectory.isEmpty())
        m_workingDirectory = QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR);
    else
        m_workingDirectory = workingDirectory;
}

QVariantMap ProcessStep::toMap() const
{
    QVariantMap map(AbstractProcessStep::toMap());
    map.insert(QLatin1String(PROCESS_COMMAND_KEY), command());
    map.insert(QLatin1String(PROCESS_ARGUMENTS_KEY), arguments());
    map.insert(QLatin1String(PROCESS_WORKINGDIRECTORY_KEY), workingDirectory());
    return map;
}

bool ProcessStep::fromMap(const QVariantMap &map)
{
    setCommand(map.value(QLatin1String(PROCESS_COMMAND_KEY)).toString());
    setArguments(map.value(QLatin1String(PROCESS_ARGUMENTS_KEY)).toString());
    setWorkingDirectory(map.value(QLatin1String(PROCESS_WORKINGDIRECTORY_KEY)).toString());
    return AbstractProcessStep::fromMap(map);
}

//*******
// ProcessStepFactory
//*******

ProcessStepFactory::ProcessStepFactory()
{
}

ProcessStepFactory::~ProcessStepFactory()
{
}

bool ProcessStepFactory::canCreate(BuildStepList *parent, const Core::Id id) const
{
    Q_UNUSED(parent);
    return id == Core::Id(PROCESS_STEP_ID);
}

BuildStep *ProcessStepFactory::create(BuildStepList *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;
    return new ProcessStep(parent);
}

bool ProcessStepFactory::canClone(BuildStepList *parent, BuildStep *bs) const
{
    return canCreate(parent, bs->id());
}

BuildStep *ProcessStepFactory::clone(BuildStepList *parent, BuildStep *bs)
{
    if (!canClone(parent, bs))
        return 0;
    return new ProcessStep(parent, static_cast<ProcessStep *>(bs));
}

bool ProcessStepFactory::canRestore(BuildStepList *parent, const QVariantMap &map) const
{
    Core::Id id = ProjectExplorer::idFromMap(map);
    return canCreate(parent, id);
}

BuildStep *ProcessStepFactory::restore(BuildStepList *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    ProcessStep *bs(new ProcessStep(parent));
    if (bs->fromMap(map))
        return bs;
    delete bs;
    return 0;
}

QList<Core::Id> ProcessStepFactory::availableCreationIds(BuildStepList *parent) const
{
    Q_UNUSED(parent);
    return QList<Core::Id>() << Core::Id(PROCESS_STEP_ID);
}
QString ProcessStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == Core::Id(PROCESS_STEP_ID))
        return ProcessStep::tr("Custom Process Step", "item in combobox");
    return QString();
}

//*******
// ProcessStepConfigWidget
//*******

ProcessStepConfigWidget::ProcessStepConfigWidget(ProcessStep *step)
        : m_step(step)
{
    m_ui.setupUi(this);
    m_ui.command->setExpectedKind(Utils::PathChooser::Command);
    m_ui.workingDirectory->setExpectedKind(Utils::PathChooser::Directory);

    BuildConfiguration *bc = m_step->buildConfiguration();
    if (!bc)
        bc = m_step->target()->activeBuildConfiguration();
    m_ui.command->setEnvironment(bc->environment());
    m_ui.command->setPath(m_step->command());

    m_ui.workingDirectory->setEnvironment(bc->environment());
    m_ui.workingDirectory->setPath(m_step->workingDirectory());

    m_ui.commandArgumentsLineEdit->setText(m_step->arguments());

    updateDetails();

    connect(m_ui.command, SIGNAL(changed(QString)),
            this, SLOT(commandLineEditTextEdited()));
    connect(m_ui.workingDirectory, SIGNAL(changed(QString)),
            this, SLOT(workingDirectoryLineEditTextEdited()));

    connect(m_ui.commandArgumentsLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(commandArgumentsLineEditTextEdited()));
}

void ProcessStepConfigWidget::updateDetails()
{
    QString displayName = m_step->displayName();
    if (displayName.isEmpty())
        displayName = tr("Custom Process Step");
    ProcessParameters param;
    BuildConfiguration *bc = m_step->buildConfiguration();
    if (!bc) // iff the step is actually in the deploy list
        bc = m_step->target()->activeBuildConfiguration();
    param.setMacroExpander(bc->macroExpander());
    param.setEnvironment(bc->environment());

    param.setWorkingDirectory(m_step->workingDirectory());
    param.setCommand(m_step->command());
    param.setArguments(m_step->arguments());
    m_summaryText = param.summary(displayName);
    emit updateSummary();
}

QString ProcessStepConfigWidget::displayName() const
{
    return m_step->displayName();
}

QString ProcessStepConfigWidget::summaryText() const
{
    return m_summaryText;
}

void ProcessStepConfigWidget::commandLineEditTextEdited()
{
    m_step->setCommand(m_ui.command->rawPath());
    updateDetails();
}

void ProcessStepConfigWidget::workingDirectoryLineEditTextEdited()
{
    m_step->setWorkingDirectory(m_ui.workingDirectory->rawPath());
}

void ProcessStepConfigWidget::commandArgumentsLineEditTextEdited()
{
    m_step->setArguments(m_ui.commandArgumentsLineEdit->text());
    updateDetails();
}

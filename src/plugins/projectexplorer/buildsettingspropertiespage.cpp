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

#include "buildsettingspropertiespage.h"
#include "buildstep.h"
#include "buildstepspage.h"
#include "project.h"
#include "projectexplorerconstants.h"
#include "target.h"
#include "buildconfiguration.h"
#include "buildconfigurationmodel.h"

#include <coreplugin/coreconstants.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/qtcassert.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/buildmanager.h>

#include <QMargins>
#include <QTimer>
#include <QCoreApplication>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace ProjectExplorer;
using namespace ProjectExplorer::Internal;

///
// BuildSettingsPanelFactory
///

QString BuildSettingsPanelFactory::id() const
{
    return QLatin1String(BUILDSETTINGS_PANEL_ID);
}

QString BuildSettingsPanelFactory::displayName() const
{
    return QCoreApplication::translate("BuildSettingsPanelFactory", "Build Settings");
}

int BuildSettingsPanelFactory::priority() const
{
    return 10;
}

bool BuildSettingsPanelFactory::supports(Target *target)
{
    return target->buildConfigurationFactory();
}

PropertiesPanel *BuildSettingsPanelFactory::createPanel(Target *target)
{
    PropertiesPanel *panel = new PropertiesPanel;
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    QWidget *b = new BuildSettingsWidget(target);
    l->addWidget(b);
    l->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    l->setContentsMargins(QMargins());
    panel->setWidget(w);
    panel->setIcon(QIcon(QLatin1String(":/projectexplorer/images/BuildSettings.png")));
    panel->setDisplayName(QCoreApplication::translate("BuildSettingsPanel", "Build Settings"));
    return panel;
}

///
// BuildSettingsWidget
///

BuildSettingsWidget::~BuildSettingsWidget()
{
    clear();
}

BuildSettingsWidget::BuildSettingsWidget(Target *target) :
    m_target(target),
    m_buildConfiguration(0)
{
    Q_ASSERT(m_target);
    setupUi();
}

void BuildSettingsWidget::setupUi()
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);

    if (!m_target->buildConfigurationFactory()) {
        QLabel *noSettingsLabel = new QLabel(this);
        noSettingsLabel->setText(tr("No build settings available"));
        QFont f = noSettingsLabel->font();
        f.setPointSizeF(f.pointSizeF() * 1.2);
        noSettingsLabel->setFont(f);
        vbox->addWidget(noSettingsLabel);
        return;
    }

    { // Edit Build Configuration row
        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setContentsMargins(0, 0, 0, 0);
        hbox->addWidget(new QLabel(tr("Edit build configuration:"), this));
        m_buildConfigurationComboBox = new QComboBox(this);
        m_buildConfigurationComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_buildConfigurationComboBox->setModel(new BuildConfigurationModel(m_target, this));
        hbox->addWidget(m_buildConfigurationComboBox);

        m_addButton = new QPushButton(this);
        m_addButton->setText(tr("Add"));
        m_addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        hbox->addWidget(m_addButton);
        m_addButtonMenu = new QMenu(this);
        m_addButton->setMenu(m_addButtonMenu);

        m_removeButton = new QPushButton(this);
        m_removeButton->setText(tr("Remove"));
        m_removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        hbox->addWidget(m_removeButton);

        m_renameButton = new QPushButton(this);
        m_renameButton->setText(tr("Rename..."));
        m_renameButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        hbox->addWidget(m_renameButton);

        hbox->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
        vbox->addLayout(hbox);
    }

    m_buildConfiguration = m_target->activeBuildConfiguration();
    BuildConfigurationModel *model = static_cast<BuildConfigurationModel *>(m_buildConfigurationComboBox->model());
    m_buildConfigurationComboBox->setCurrentIndex(model->indexFor(m_buildConfiguration).row());

    updateAddButtonMenu();
    updateBuildSettings();

    connect(m_buildConfigurationComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIndexChanged(int)));

    connect(m_removeButton, SIGNAL(clicked()),
            this, SLOT(deleteConfiguration()));

    connect(m_renameButton, SIGNAL(clicked()),
            this, SLOT(renameConfiguration()));

    connect(m_target, SIGNAL(activeBuildConfigurationChanged(ProjectExplorer::BuildConfiguration*)),
            this, SLOT(updateActiveConfiguration()));

    if (m_target->buildConfigurationFactory())
        connect(m_target->buildConfigurationFactory(), SIGNAL(availableCreationIdsChanged()),
                SLOT(updateAddButtonMenu()));
}

void BuildSettingsWidget::addSubWidget(BuildConfigWidget *widget)
{
    widget->setContentsMargins(0, 10, 0, 0);

    QLabel *label = new QLabel(this);
    label->setText(widget->displayName());
    connect(widget, SIGNAL(displayNameChanged(QString)),
            label, SLOT(setText(QString)));
    QFont f = label->font();
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() * 1.2);
    label->setFont(f);

    label->setContentsMargins(0, 10, 0, 0);

    layout()->addWidget(label);
    layout()->addWidget(widget);

    m_labels.append(label);
    m_subWidgets.append(widget);
}

void BuildSettingsWidget::clear()
{
    qDeleteAll(m_subWidgets);
    m_subWidgets.clear();
    qDeleteAll(m_labels);
    m_labels.clear();
}

QList<BuildConfigWidget *> BuildSettingsWidget::subWidgets() const
{
    return m_subWidgets;
}

void BuildSettingsWidget::updateAddButtonMenu()
{
    m_addButtonMenu->clear();
    if (m_target) {
        if (m_target->activeBuildConfiguration()) {
            m_addButtonMenu->addAction(tr("&Clone Selected"),
                                       this, SLOT(cloneConfiguration()));
        }
        IBuildConfigurationFactory *factory = m_target->buildConfigurationFactory();
        if (factory) {
            foreach (Core::Id id, factory->availableCreationIds(m_target)) {
                QAction *action = m_addButtonMenu->addAction(factory->displayNameForId(id), this, SLOT(createConfiguration()));
                action->setData(QVariant::fromValue(id));
            }
        }
    }
}

void BuildSettingsWidget::updateBuildSettings()
{
    clear();

    // update buttons
    m_removeButton->setEnabled(m_target->buildConfigurations().size() > 1);

    // Add pages
    BuildConfigWidget *generalConfigWidget = m_target->createConfigWidget();
    addSubWidget(generalConfigWidget);

    addSubWidget(new BuildStepsPage(m_target, Core::Id(Constants::BUILDSTEPS_BUILD)));
    addSubWidget(new BuildStepsPage(m_target, Core::Id(Constants::BUILDSTEPS_CLEAN)));

    QList<BuildConfigWidget *> subConfigWidgets = m_target->project()->subConfigWidgets();
    foreach (BuildConfigWidget *subConfigWidget, subConfigWidgets)
        addSubWidget(subConfigWidget);

    foreach (BuildConfigWidget *widget, subWidgets())
        widget->init(m_buildConfiguration);
}

void BuildSettingsWidget::currentIndexChanged(int index)
{
    BuildConfigurationModel *model = static_cast<BuildConfigurationModel *>(m_buildConfigurationComboBox->model());
    BuildConfiguration *buildConfiguration = model->buildConfigurationAt(index);
    m_target->setActiveBuildConfiguration(buildConfiguration);
}

void BuildSettingsWidget::updateActiveConfiguration()
{
    if (!m_buildConfiguration || m_buildConfiguration == m_target->activeBuildConfiguration())
        return;

    m_buildConfiguration = m_target->activeBuildConfiguration();

    BuildConfigurationModel *model = static_cast<BuildConfigurationModel *>(m_buildConfigurationComboBox->model());
    m_buildConfigurationComboBox->setCurrentIndex(model->indexFor(m_buildConfiguration).row());

    foreach (QWidget *widget, subWidgets()) {
        if (BuildConfigWidget *buildStepWidget = qobject_cast<BuildConfigWidget*>(widget)) {
            buildStepWidget->init(m_buildConfiguration);
        }
    }
}

void BuildSettingsWidget::createConfiguration()
{
    if (!m_target->buildConfigurationFactory())
        return;

    QAction *action = qobject_cast<QAction *>(sender());
    Core::Id id = action->data().value<Core::Id>();
    BuildConfiguration *bc = m_target->buildConfigurationFactory()->create(m_target, id);
    if (bc) {
        m_target->setActiveBuildConfiguration(bc);
        updateBuildSettings();
    }
}

void BuildSettingsWidget::cloneConfiguration()
{
    cloneConfiguration(m_buildConfiguration);
}

void BuildSettingsWidget::deleteConfiguration()
{
    deleteConfiguration(m_buildConfiguration);
}

QString BuildSettingsWidget::uniqueName(const QString & name)
{
    QString result = name.trimmed();
    if (!result.isEmpty()) {
        QStringList bcNames;
        foreach (BuildConfiguration *bc, m_target->buildConfigurations()) {
            if (bc == m_buildConfiguration)
                continue;
            bcNames.append(bc->displayName());
        }
        result = Project::makeUnique(result, bcNames);
    }
    return result;
}

void BuildSettingsWidget::renameConfiguration()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Rename..."),
                                         tr("New name for build configuration <b>%1</b>:").
                                            arg(m_buildConfiguration->displayName()),
                                         QLineEdit::Normal,
                                         m_buildConfiguration->displayName(), &ok);
    if (!ok)
        return;

    name = uniqueName(name);
    if (name.isEmpty())
        return;

    m_buildConfiguration->setDisplayName(name);

}

void BuildSettingsWidget::cloneConfiguration(BuildConfiguration *sourceConfiguration)
{
    if (!sourceConfiguration ||
        !m_target->buildConfigurationFactory())
        return;

    //: Title of a the cloned BuildConfiguration window, text of the window
    QString name = uniqueName(QInputDialog::getText(this, tr("Clone Configuration"), tr("New configuration name:")));
    if (name.isEmpty())
        return;

    BuildConfiguration * bc(m_target->buildConfigurationFactory()->clone(m_target, sourceConfiguration));
    if (!bc)
        return;

    bc->setDisplayName(name);
    m_target->addBuildConfiguration(bc);
    updateBuildSettings();

    m_target->setActiveBuildConfiguration(bc);
}

void BuildSettingsWidget::deleteConfiguration(BuildConfiguration *deleteConfiguration)
{
    if (!deleteConfiguration ||
        m_target->buildConfigurations().size() <= 1)
        return;

    ProjectExplorer::BuildManager *bm = ProjectExplorerPlugin::instance()->buildManager();
    if (bm->isBuilding(deleteConfiguration)) {
        QMessageBox box;
        QPushButton *closeAnyway = box.addButton(tr("Cancel Build && Remove Build Configuration"), QMessageBox::AcceptRole);
        QPushButton *cancelClose = box.addButton(tr("Do Not Remove"), QMessageBox::RejectRole);
        box.setDefaultButton(cancelClose);
        box.setWindowTitle(tr("Remove Build Configuration %1?").arg(deleteConfiguration->displayName()));
        box.setText(tr("The build configuration <b>%1</b> is currently being built.").arg(deleteConfiguration->displayName()));
        box.setInformativeText(tr("Do you want to cancel the build process and remove the Build Configuration anyway?"));
        box.exec();
        if (box.clickedButton() != closeAnyway)
            return;
        bm->cancel();
    } else {
        QMessageBox msgBox(QMessageBox::Question, tr("Remove Build Configuration?"),
                           tr("Do you really want to delete build configuration <b>%1</b>?").arg(deleteConfiguration->displayName()),
                           QMessageBox::Yes|QMessageBox::No, this);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setEscapeButton(QMessageBox::No);
        if (msgBox.exec() == QMessageBox::No)
            return;
    }

    m_target->removeBuildConfiguration(deleteConfiguration);

    updateBuildSettings();
}

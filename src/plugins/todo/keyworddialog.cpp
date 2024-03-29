/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Dmitry Savchenko.
** Copyright (c) 2010 Vasiliy Sorokin.
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

#include "keyworddialog.h"
#include "keyword.h"
#include "ui_keyworddialog.h"
#include "constants.h"

#include <QColorDialog>

namespace Todo {
namespace Internal {

KeywordDialog::KeywordDialog(const Keyword &keyword, const QSet<QString> &alreadyUsedKeywordNames,
                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeywordDialog),
    m_alreadyUsedKeywordNames(alreadyUsedKeywordNames)
{
    ui->setupUi(this);
    setupListWidget(keyword.iconResource);
    setupColorWidgets(keyword.color);
    ui->keywordNameEdit->setText(keyword.name);
    ui->errorLabel->hide();

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(acceptButtonClicked()));
    connect(ui->keywordNameEdit, SIGNAL(textChanged(QString)), ui->errorLabel, SLOT(hide()));
}

KeywordDialog::~KeywordDialog()
{
    delete ui;
}

Keyword KeywordDialog::keyword()
{
    Keyword result;
    result.name = keywordName();
    result.iconResource = ui->listWidget->currentItem()->data(Qt::UserRole).toString();
    result.color = ui->colorEdit->text();

    return result;
}

void KeywordDialog::colorSelected(const QColor &color)
{
    ui->colorEdit->setText(color.name());
}

void KeywordDialog::acceptButtonClicked()
{
    if (canAccept())
        accept();
}

void KeywordDialog::setupListWidget(const QString &selectedIcon)
{
    ui->listWidget->setViewMode(QListWidget::IconMode);

    const QString infoIconName = QLatin1String(Constants::ICON_INFO);
    QListWidgetItem *item = new QListWidgetItem(QIcon(infoIconName),
                                                QLatin1String("information"));
    item->setData(Qt::UserRole, infoIconName);
    ui->listWidget->addItem(item);

    const QString warningIconName = QLatin1String(Constants::ICON_WARNING);
    item = new QListWidgetItem(QIcon(warningIconName),
                               QLatin1String("warning"));
    item->setData(Qt::UserRole, warningIconName);
    ui->listWidget->addItem(item);

    const QString errorIconName = QLatin1String(Constants::ICON_ERROR);
    item = new QListWidgetItem(QIcon(errorIconName),
                               QLatin1String("error"));
    item->setData(Qt::UserRole, errorIconName);
    ui->listWidget->addItem(item);

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        item = ui->listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == selectedIcon) {
            ui->listWidget->setCurrentItem(item);
            break;
        }
    }
}

void KeywordDialog::setupColorWidgets(const QColor &color)
{
    ui->colorButton->setColor(color);
    ui->colorEdit->setText(color.name());
    connect(ui->colorButton, SIGNAL(colorChanged(QColor)), SLOT(colorSelected(QColor)));
}

bool KeywordDialog::canAccept()
{
    if (!isKeywordNameCorrect()) {
        showError(tr("Keyword cannot be empty, contain spaces or colons."));
        return false;
    }

    if (isKeywordNameAlreadyUsed()) {
        showError(tr("There is already a keyword with this name."));
        return false;
    }

    return true;
}

bool KeywordDialog::isKeywordNameCorrect()
{
    // Make sure keyword is not empty and contains no spaces or colons

    QString name = keywordName();

    if (name.isEmpty())
        return false;

    for (int i = 0; i < name.size(); ++i)
        if (name.at(i).isSpace() || name.at(i) == QLatin1Char(':'))
            return false;

    return true;
}

bool KeywordDialog::isKeywordNameAlreadyUsed()
{
    return m_alreadyUsedKeywordNames.contains(keywordName());
}

void KeywordDialog::showError(const QString &text)
{
    ui->errorLabel->setText(text);
    ui->errorLabel->show();
}

QString KeywordDialog::keywordName()
{
    return ui->keywordNameEdit->text().trimmed();
}

} // namespace Internal
} // namespace Todo

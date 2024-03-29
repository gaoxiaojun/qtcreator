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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "icontext.h"
#include "icore.h"

#include <utils/appmainwindow.h>

#include <QMap>
#include <QColor>

QT_BEGIN_NAMESPACE
class QSettings;
class QShortcut;
class QPrinter;
class QToolButton;
QT_END_NAMESPACE

namespace Core {

class ActionManager;
class StatusBarWidget;
class EditorManager;
class ExternalToolManager;
class DocumentManager;
class HelpManager;
class IWizard;
class MessageManager;
class MimeDatabase;
class ModeManager;
class ProgressManager;
class NavigationWidget;
class RightPaneWidget;
class ScriptManager;
class SettingsDatabase;
class VariableManager;
class VcsManager;

namespace Internal {

class ActionManagerPrivate;
class FancyTabWidget;
class GeneralSettings;
class ProgressManagerPrivate;
class ShortcutSettings;
class ToolSettings;
class MimeTypeSettings;
class StatusBarManager;
class VersionDialog;
class SystemEditor;

class MainWindow : public Utils::AppMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    bool init(QString *errorMessage);
    void extensionsInitialized();
    void aboutToShutdown();

    IContext *contextObject(QWidget *widget);
    void addContextObject(IContext *contex);
    void removeContextObject(IContext *contex);
    void resetContext();

    void openFiles(const QStringList &fileNames, ICore::OpenFilesFlags flags);

    Core::ActionManager *actionManager() const;
    Core::MessageManager *messageManager() const;
    Core::EditorManager *editorManager() const;
    Core::ProgressManager *progressManager() const;
    Core::ScriptManager *scriptManager() const;
    Core::VariableManager *variableManager() const;
    Core::ModeManager *modeManager() const;
    Core::MimeDatabase *mimeDatabase() const;
    Core::HelpManager *helpManager() const;

    VcsManager *vcsManager() const;
    QSettings *settings(QSettings::Scope scope) const;
    inline SettingsDatabase *settingsDatabase() const { return m_settingsDatabase; }
    virtual QPrinter *printer() const;
    IContext * currentContextObject() const;
    QStatusBar *statusBar() const;

    void updateAdditionalContexts(const Context &remove, const Context &add);
    bool hasContext(int context) const;

    void setSuppressNavigationWidget(bool suppress);

    void setOverrideColor(const QColor &color);

    bool isPresentationModeEnabled();
    void setPresentationModeEnabled(bool);

#ifdef Q_OS_MAC
    void setIsFullScreen(bool fullScreen);
#endif
signals:
    void windowActivated();

public slots:
    void newFile();
    void openFileWith();
    void exit();
    void setFullScreen(bool on);

    void showNewItemDialog(const QString &title,
                           const QList<IWizard *> &wizards,
                           const QString &defaultLocation = QString(),
                           const QVariantMap &extraVariables = QVariantMap());

    bool showOptionsDialog(const QString &category = QString(),
                           const QString &page = QString(),
                           QWidget *parent = 0);

    bool showWarningWithOptions(const QString &title, const QString &text,
                                const QString &details = QString(),
                                const QString &settingsCategory = QString(),
                                const QString &settingsId = QString(),
                                QWidget *parent = 0);

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

private slots:
    void openFile();
    void aboutToShowRecentFiles();
    void openRecentFile();
    void setFocusToEditor();
    void saveAll();
    void aboutQtCreator();
    void aboutPlugins();
    void updateFocusWidget(QWidget *old, QWidget *now);
    void setSidebarVisible(bool visible);
    void destroyVersionDialog();
    void openDelayedFiles();

private:
    void updateContextObject(IContext *context);
    void updateContext();

    void registerDefaultContainers();
    void registerDefaultActions();

    void readSettings();
    void writeSettings();

    ICore *m_coreImpl;
    Context m_additionalContexts;
    QSettings *m_settings;
    QSettings *m_globalSettings;
    SettingsDatabase *m_settingsDatabase;
    mutable QPrinter *m_printer;
    ActionManagerPrivate *m_actionManager;
    EditorManager *m_editorManager;
    ExternalToolManager *m_externalToolManager;
    MessageManager *m_messageManager;
    ProgressManagerPrivate *m_progressManager;
    ScriptManager *m_scriptManager;
    QScopedPointer<VariableManager> m_variableManager;
    VcsManager *m_vcsManager;
    StatusBarManager *m_statusBarManager;
    ModeManager *m_modeManager;
    MimeDatabase *m_mimeDatabase;
    HelpManager *m_helpManager;
    FancyTabWidget *m_modeStack;
    NavigationWidget *m_navigationWidget;
    RightPaneWidget *m_rightPaneWidget;
    Core::StatusBarWidget *m_outputView;
    VersionDialog *m_versionDialog;

    IContext *m_activeContext;

    QMap<QWidget *, IContext *> m_contextWidgets;

    GeneralSettings *m_generalSettings;
    ShortcutSettings *m_shortcutSettings;
    ToolSettings *m_toolSettings;
    MimeTypeSettings *m_mimeTypeSettings;
    SystemEditor *m_systemEditor;

    // actions
    QShortcut *m_focusToEditor;
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_openWithAction;
    QAction *m_saveAllAction;
    QAction *m_exitAction;
    QAction *m_optionsAction;
    QAction *m_toggleSideBarAction;
    QAction *m_toggleFullScreenAction;
#ifdef Q_OS_MAC
    QAction *m_minimizeAction;
    QAction *m_zoomAction;
#endif

    QToolButton *m_toggleSideBarButton;
    QColor m_overrideColor;

    QStringList m_filesToOpenDelayed;
    QTimer *m_autoSaveSessionTimer;
};

} // namespace Internal
} // namespace Core

#endif // MAINWINDOW_H

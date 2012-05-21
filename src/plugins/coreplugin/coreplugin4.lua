dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/shared/scriptwrapper/scriptwrapper4.lua")

project "Core"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")

scriptwrapper.Configure()

defines
{
      "CORE_LIBRARY"
}

qt4.QT
{
      "core"
    , "gui"
    , "help"
    , "network"
    , "script"
    , "sql"
}

includedirs
{
      "dialogs"
    , "actionmanager"
    , "editormanager"
    , "progressmanager"
    , "scriptmanager"
    , ".."
    , "."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src"
}

files
{
      "mainwindow.cpp"
    , "editmode.cpp"
    , "tabpositionindicator.cpp"
    , "fancyactionbar.cpp"
    , "fancytabwidget.cpp"
    , "generalsettings.cpp"
    , "id.cpp"
    , "icontext.cpp"
    , "messagemanager.cpp"
    , "messageoutputwindow.cpp"
    , "outputpane.cpp"
    , "outputwindow.cpp"
    , "vcsmanager.cpp"
    , "statusbarmanager.cpp"
    , "versiondialog.cpp"
    , "editormanager/editormanager.cpp"
    , "editormanager/editorview.cpp"
    , "editormanager/openeditorsmodel.cpp"
    , "editormanager/openeditorsview.cpp"
    , "editormanager/openeditorswindow.cpp"
    , "editormanager/ieditorfactory.cpp"
    , "editormanager/iexternaleditor.cpp"
    , "actionmanager/actionmanager.cpp"
    , "actionmanager/command.cpp"
    , "actionmanager/actioncontainer.cpp"
    , "actionmanager/commandsfile.cpp"
    , "dialogs/saveitemsdialog.cpp"
    , "dialogs/newdialog.cpp"
    , "dialogs/settingsdialog.cpp"
    , "actionmanager/commandmappings.cpp"
    , "dialogs/shortcutsettings.cpp"
    , "dialogs/openwithdialog.cpp"
    , "progressmanager/progressmanager.cpp"
    , "progressmanager/progressview.cpp"
    , "progressmanager/progressbar.cpp"
    , "progressmanager/futureprogress.cpp"
    , "scriptmanager/scriptmanager.cpp"
    , "statusbarwidget.cpp"
    , "coreplugin.cpp"
    , "variablemanager.cpp"
    , "modemanager.cpp"
    , "basefilewizard.cpp"
    , "generatedfile.cpp"
    , "plugindialog.cpp"
    , "inavigationwidgetfactory.cpp"
    , "navigationwidget.cpp"
    , "manhattanstyle.cpp"
    , "minisplitter.cpp"
    , "styleanimator.cpp"
    , "findplaceholder.cpp"
    , "rightpane.cpp"
    , "sidebar.cpp"
    , "fileiconprovider.cpp"
    , "mimedatabase.cpp"
    , "icore.cpp"
    , "infobar.cpp"
    , "editormanager/ieditor.cpp"
    , "dialogs/ioptionspage.cpp"
    , "dialogs/iwizard.cpp"
    , "settingsdatabase.cpp"
    , "imode.cpp"
    , "editormanager/systemeditor.cpp"
    , "designmode.cpp"
    , "editortoolbar.cpp"
    , "helpmanager.cpp"
    , "outputpanemanager.cpp"
    , "navigationsubwidget.cpp"
    , "sidebarwidget.cpp"
    , "externaltool.cpp"
    , "dialogs/externaltoolconfig.cpp"
    , "toolsettings.cpp"
    , "variablechooser.cpp"
    , "mimetypemagicdialog.cpp"
    , "mimetypesettings.cpp"
    , "dialogs/promptoverwritedialog.cpp"
    , "fileutils.cpp"
    , "featureprovider.cpp"
    , "idocument.cpp"
    , "textdocument.cpp"
    , "documentmanager.cpp"
}


qt4.HEADERS
{
      "mainwindow.h"
    , "editmode.h"
    , "tabpositionindicator.h"
    , "fancyactionbar.h"
    , "fancytabwidget.h"
    , "generalsettings.h"
    , "id.h"
    , "messagemanager.h"
    , "messageoutputwindow.h"
    , "outputpane.h"
    , "outputwindow.h"
    , "vcsmanager.h"
    , "statusbarmanager.h"
    , "editormanager/editormanager.h"
    , "editormanager/editorview.h"
    , "editormanager/openeditorsmodel.h"
    , "editormanager/openeditorsview.h"
    , "editormanager/openeditorswindow.h"
    , "editormanager/ieditor.h"
    , "editormanager/iexternaleditor.h"
    , "editormanager/ieditorfactory.h"
    , "actionmanager/actioncontainer.h"
    , "actionmanager/actionmanager.h"
    , "actionmanager/command.h"
    , "actionmanager/actionmanager_p.h"
    , "actionmanager/command_p.h"
    , "actionmanager/actioncontainer_p.h"
    , "actionmanager/commandsfile.h"
    , "dialogs/saveitemsdialog.h"
    , "dialogs/newdialog.h"
    , "dialogs/settingsdialog.h"
    , "actionmanager/commandmappings.h"
    , "dialogs/shortcutsettings.h"
    , "dialogs/openwithdialog.h"
    , "dialogs/iwizard.h"
    , "dialogs/ioptionspage.h"
    , "progressmanager/progressmanager_p.h"
    , "progressmanager/progressview.h"
    , "progressmanager/progressbar.h"
    , "progressmanager/futureprogress.h"
    , "progressmanager/progressmanager.h"
    , "icontext.h"
    , "icore.h"
    , "infobar.h"
    , "imode.h"
    , "ioutputpane.h"
    , "coreconstants.h"
    , "iversioncontrol.h"
    , "ifilewizardextension.h"
    , "icorelistener.h"
    , "versiondialog.h"
    , "scriptmanager/metatypedeclarations.h"
    , "scriptmanager/scriptmanager.h"
    , "scriptmanager/scriptmanager_p.h"
    , "core_global.h"
    , "statusbarwidget.h"
    , "coreplugin.h"
    , "variablemanager.h"
    , "modemanager.h"
    , "basefilewizard.h"
    , "generatedfile.h"
    , "plugindialog.h"
    , "inavigationwidgetfactory.h"
    , "navigationwidget.h"
    , "manhattanstyle.h"
    , "minisplitter.h"
    , "styleanimator.h"
    , "findplaceholder.h"
    , "rightpane.h"
    , "sidebar.h"
    , "fileiconprovider.h"
    , "mimedatabase.h"
    , "settingsdatabase.h"
    , "editormanager/systemeditor.h"
    , "designmode.h"
    , "editortoolbar.h"
    , "helpmanager.h"
    , "outputpanemanager.h"
    , "navigationsubwidget.h"
    , "sidebarwidget.h"
    , "externaltool.h"
    , "dialogs/externaltoolconfig.h"
    , "toolsettings.h"
    , "variablechooser.h"
    , "mimetypemagicdialog.h"
    , "mimetypesettings.h"
    , "dialogs/promptoverwritedialog.h"
    , "fileutils.h"
    , "externaltoolmanager.h"
    , "generatedfile.h"
    , "featureprovider.h"
    , "idocument.h"
    , "idocumentfactory.h"
    , "textdocument.h"
    , "documentmanager.h"
    , "progressmanager/futureprogress.cpp"
    , "basefilewizard.cpp"
    , "mimetypesettings.cpp"
}


if (os.is("windows")) then
    files
    {
        "progressmanager/progressmanager_win.cpp"
    }
    links
    {
        "ole32"
    }
end


if (os.is("linux")) then
    files
    {
        "progressmanager/progressmanager_x11.cpp"
    }
end


qt4.FORMS
{
      "dialogs/newdialog.ui"
    , "actionmanager/commandmappings.ui"
    , "dialogs/saveitemsdialog.ui"
    , "dialogs/openwithdialog.ui"
    , "editormanager/openeditorsview.ui"
    , "generalsettings.ui"
    , "dialogs/externaltoolconfig.ui"
    , "variablechooser.ui"
    , "mimetypesettingspage.ui"
    , "mimetypemagicdialog.ui"
}


qt4.RESOURCES
{
      "core.qrc"
    , "fancyactionbar.qrc"
}

links { "Utils", "aggregation", "extensionsystem" }

qt4.INSTALL
{
      "Core.pluginspec"
	, solution().basedir .. "/bin/libCore*.so"
	, solution().basedir .. "/bin/Core*.dll"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE( "Core.pluginspec.in" )

qt4.Configure()
Configure()



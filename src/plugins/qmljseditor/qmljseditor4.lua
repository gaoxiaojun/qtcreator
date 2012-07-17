dofile (solution().basedir .. "/build/qt4.lua")

project "QmlJSEditor"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.QT
{
      "core"
    , "gui"
    , "script"
}

defines
{
      "QMLJSEDITOR_LIBRARY"
    , "QT_CREATOR"
}


qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL
{
      "QmlJSEditor.pluginspec"
    , solution().basedir .. "/bin/libQmlJSEditor*.so"
    , solution().basedir .. "/bin/QmlJSEditor*.dll"
}


includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src"
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/libs/3rdparty"
    , solution().basedir .. "/src/libs/3rdparty/cplusplus"
    , solution().basedir .. "/src/libs/qmleditorwidgets"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("QmlJSEditor.pluginspec.in")


qt4.HEADERS
{
      "qmljseditor.h"
    , "qmljseditor_global.h"
    , "qmljseditoractionhandler.h"
    , "qmljseditorconstants.h"
    , "qmljseditorfactory.h"
    , "qmljseditorplugin.h"
    , "qmlexpressionundercursor.h"
    , "qmlfilewizard.h"
    , "qmljshighlighter.h"
    , "qmljshoverhandler.h"
    , "qmljspreviewrunner.h"
    , "qmljscomponentfromobjectdef.h"
    , "qmljsoutline.h"
    , "qmloutlinemodel.h"
    , "qmltaskmanager.h"
    , "qmljsoutlinetreeview.h"
    , "quicktoolbarsettingspage.h"
    , "quicktoolbar.h"
    , "qmljscomponentnamedialog.h"
    , "qmljsfindreferences.h"
    , "qmljseditoreditable.h"
    , "qmljsautocompleter.h"
    , "jsfilewizard.h"
    , "qmljssnippetprovider.h"
    , "qmljsreuse.h"
    , "qmljsquickfixassist.h"
    , "qmljscompletionassist.h"
    , "qmljsquickfix.h"
    , "qmljssemanticinfoupdater.h"
    , "qmljssemantichighlighter.h"
    , "qmljswrapinloader.h"
    , "jsfilewizard.cpp"
}


files
{
      "qmljseditor.cpp"
    , "qmljseditoractionhandler.cpp"
    , "qmljseditorfactory.cpp"
    , "qmljseditorplugin.cpp"
    , "qmlexpressionundercursor.cpp"
    , "qmlfilewizard.cpp"
    , "qmljshighlighter.cpp"
    , "qmljshoverhandler.cpp"
    , "qmljspreviewrunner.cpp"
    , "qmljscomponentfromobjectdef.cpp"
    , "qmljsoutline.cpp"
    , "qmloutlinemodel.cpp"
    , "qmltaskmanager.cpp"
    , "qmljsquickfixes.cpp"
    , "qmljsoutlinetreeview.cpp"
    , "quicktoolbarsettingspage.cpp"
    , "quicktoolbar.cpp"
    , "qmljscomponentnamedialog.cpp"
    , "qmljsfindreferences.cpp"
    , "qmljseditoreditable.cpp"
    , "qmljsautocompleter.cpp"
    , "jsfilewizard.cpp"
    , "qmljssnippetprovider.cpp"
    , "qmljsreuse.cpp"
    , "qmljsquickfixassist.cpp"
    , "qmljscompletionassist.cpp"
    , "qmljsquickfix.cpp"
    , "qmljssemanticinfoupdater.cpp"
    , "qmljssemantichighlighter.cpp"
    , "qmljswrapinloader.cpp"
}


links
{
      "Utils"
    , "QmlJSTools"
    , "Core"
    , "Find"
    , "TextEditor"
    , "Locator"
    , "extensionsystem"
    , "aggregation"
    , "QmlEditorWidgets"
	, "QmlJS"
}

qt4.RESOURCES { "qmljseditor.qrc" }
--files { "QmlJSEditor.mimetypes.xml" }

qt4.FORMS
{
      "quicktoolbarsettingspage.ui"
    , "qmljscomponentnamedialog.ui"
}

qt4.Configure()
Configure()


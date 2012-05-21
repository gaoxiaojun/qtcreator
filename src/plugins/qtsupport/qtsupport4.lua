dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/shared/proparser/proparser4.lua")

project "QtSupport"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.QT
{
      "declarative"
    , "core"
    , "gui"
    , "network"
}

includedirs 
{
      "."
    , ".."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/shared"
}

defines 
{ 
      "QT_CREATOR"
    , "QTSUPPORT_LIBRARY" 
    , "PROPARSER_AS_LIBRARY"
    , "PROPARSER_LIBRARY"
    , "PROPARSER_THREAD_SAFE"
    , "PROEVALUATOR_THREAD_SAFE"
    , "PROEVALUATOR_CUMULATIVE"
    , "QT_NO_CAST_TO_ASCII"
}

--include(../../qtcreatorplugin.pri)
-- include(qtsupport_dependencies.pri)

qt4.HEADERS
{
      "qtsupportplugin.h"
    , "qtsupport_global.h"
    , "qtoutputformatter.h"
    , "qtversionmanager.h"
    , "qtversionfactory.h"
    , "baseqtversion.h"
    , "qmldumptool.h"
    , "qmlobservertool.h"
    , "qmldebugginglibrary.h"
    , "qtoptionspage.h"
    , "debugginghelperbuildtask.h"
    , "qtsupportconstants.h"
    , "profilereader.h"
    , "qtparser.h"
    , "gettingstartedwelcomepage.h"
    , "exampleslistmodel.h"
    , "screenshotcropper.h"
}


files
{
      "qtsupportplugin.cpp"
    , "qtoutputformatter.cpp"
    , "qtversionmanager.cpp"
    , "qtversionfactory.cpp"
    , "baseqtversion.cpp"
    , "qmldumptool.cpp"
    , "qmlobservertool.cpp"
    , "qmldebugginglibrary.cpp"
    , "qtoptionspage.cpp"
    , "debugginghelperbuildtask.cpp"
    , "profilereader.cpp"
    , "qtparser.cpp"
    , "gettingstartedwelcomepage.cpp"
    , "exampleslistmodel.cpp"
    , "screenshotcropper.cpp"
}

qt4.FORMS
{
      "showbuildlog.ui"
    , "qtversioninfo.ui"
    , "debugginghelper.ui"
    , "qtversionmanager.ui"
}

links
{
      "QmlJS"
    , "Utils"
    , "ProjectExplorer"
    , "Core"
    , "Locator"
    , "Find"
    , "TextEditor"
    , "extensionsystem"
    , "aggregation"
}

substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("QtSupport.pluginspec.in")

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")

qt4.INSTALL
{
      "QtSupport.pluginspec"
    , solution().basedir .. "/bin/libQtSupport*.so"
    , solution().basedir .. "/bin/QtSupport*.dll" 
}

proparser.Configure()
qt4.Configure()
Configure()


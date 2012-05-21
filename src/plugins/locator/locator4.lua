dofile (solution().basedir .. "/build/qt4.lua")

project("Locator")
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.QT
{
      "core"
    , "gui"
}

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")

defines { "LOCATOR_LIBRARY" }

includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src"
    , solution().basedir .. "/src/libs"
}

qt4.HEADERS
{
      "locatorplugin.h"
    , "commandlocator.h"
    , "locatorwidget.h"
    , "locatorfiltersfilter.h"
    , "settingspage.h"
    , "ilocatorfilter.h"
    , "opendocumentsfilter.h"
    , "filesystemfilter.h"
    , "locatorconstants.h"
    , "directoryfilter.h"
    , "locatormanager.h"
    , "basefilefilter.h"
    , "locator_global.h"
    , "executefilter.h"
}


files
{
      "locatorplugin.cpp"
    , "commandlocator.cpp"
    , "locatorwidget.cpp"
    , "locatorfiltersfilter.cpp"
    , "opendocumentsfilter.cpp"
    , "filesystemfilter.cpp"
    , "settingspage.cpp"
    , "directoryfilter.cpp"
    , "locatormanager.cpp"
    , "basefilefilter.cpp"
    , "ilocatorfilter.cpp"
    , "executefilter.cpp"
}

links
{
      "Core"
    , "Utils"
    , "extensionsystem"
    , "aggregation"
}

qt4.INSTALL 
{ 
      "Locator.pluginspec"
    , solution().basedir .. "/bin/libLocator*.so" 
    , solution().basedir .. "/bin/Locator*.dll" 
}

qt4.FORMS { "settingspage.ui", "filesystemfilter.ui", "directoryfilter.ui" }
qt4.RESOURCES { "locator.qrc" }

substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("Locator.pluginspec.in")

qt4.Configure()
Configure()


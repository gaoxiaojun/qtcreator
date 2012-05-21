dofile (solution().basedir .. "/build/qt4.lua")

project "HelloWorld"
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

includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src"
    , solution().basedir .. "/src/libs"
}

qt4.HEADERS 
{
      "helloworldplugin.h"
    , "helloworldwindow.h"
}


files 
{
      "helloworldplugin.cpp"
    , "helloworldwindow.cpp"
}

links
{
      "Utils"
    , "Core"
    , "extensionsystem"
    , "aggregation"
}

substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("HelloWorld.pluginspec.in")

qt4.INSTALL
{
      "HelloWorld.pluginspec"
    , solution().basedir .. "/bin/libHelloWorld*.so"
    , solution().basedir .. "/bin/HelloWorld*.dll"
}

qt4.Configure()
Configure()

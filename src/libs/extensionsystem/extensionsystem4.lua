
dofile (solution().basedir .. "/build/qt4.lua")

project "extensionsystem"
kind "SharedLib"
language "C++"

qt4.QtPre()

defines
{
      "EXTENSIONSYSTEM_LIBRARY"
    , "IDE_TEST_DIR=\"\\\"" .. os.getenv("IDE_SOURCE_TREE") .. "\\\"\""
}

includedirs
{
    ".."
}

qt4.HEADERS
{
      "pluginerrorview.h"
    , "plugindetailsview.h"
    , "iplugin.h"
    , "iplugin_p.h"
    , "extensionsystem_global.h"
    , "pluginmanager.h"
    , "pluginmanager_p.h"
    , "pluginspec.h"
    , "pluginspec_p.h"
    , "pluginview.h"
    , "pluginview_p.h"
    , "optionsparser.h"
    , "plugincollection.h"
    , "pluginerroroverview.h"
    , "pluginerroroverview.cpp"
}


files
{
      "pluginerrorview.cpp"
    , "plugindetailsview.cpp"
    , "invoker.cpp"
    , "iplugin.cpp"
    , "pluginmanager.cpp"
    , "pluginspec.cpp"
    , "pluginview.cpp"
    , "optionsparser.cpp"
    , "plugincollection.cpp"
    , "pluginerroroverview.cpp"
}

qt4.FORMS
{
      "pluginview.ui"
    , "pluginerrorview.ui"
    , "plugindetailsview.ui"
    , "pluginerroroverview.ui"
}

qt4.RESOURCES
{
    "pluginview.qrc"
}

qt4.QT
{
      "Core"
    , "Gui"
}

if (os.is("linux")) then
    links { "dl" }
end

qt4.Configure()
Configure()


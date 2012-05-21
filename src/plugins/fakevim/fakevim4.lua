dofile (solution().basedir .. "/build/qt4.lua")

project "FakeVim"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }
qt4.QT { "core", "gui" }

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL 
{ 
      "FakeVim.pluginspec"
    , solution().basedir .. "/bin/libFakeVim*.so" 
    , solution().basedir .. "/bin/FakeVim*.dll" 
}



includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/plugins"
}

links
{
      "Core"
    , "TextEditor"
    , "Find"
    , "Locator"
}

files
{
      "fakevimactions.cpp"
    , "fakevimhandler.cpp"
    , "fakevimplugin.cpp"
}

qt4.HEADERS 
{
      "fakevimactions.h"
    , "fakevimhandler.h"
    , "fakevimplugin.h"
    , "fakevimhandler.cpp"
    , "fakevimplugin.cpp"
}

qt4.FORMS { "fakevimoptions.ui" }

substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("FakeVim.pluginspec.in")

qt4.Configure()
Configure()

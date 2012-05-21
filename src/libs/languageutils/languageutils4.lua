dofile (solution().basedir .. "/build/qt4.lua")

project "LanguageUtils"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.QT { "core", "gui" }

defines 
{
    "LANGUAGEUTILS_BUILD_DIR", "QT_CREATOR"
}

includedirs 
{
    ".."
}

qt4.HEADERS 
{
      "languageutils_global.h"
    , "fakemetaobject.h"
    , "componentversion.h"
}

files
{
      "fakemetaobject.cpp"
    , "componentversion.cpp"
}

links
{
      "Utils"
    , "Botan"
}

qt4.Configure()
Configure()

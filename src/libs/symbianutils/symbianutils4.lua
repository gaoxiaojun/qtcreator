dofile (solution().basedir .. "/build/qt4.lua")
project "symbianutils4.lua"
language "C++"
kind "SharedLib"

qt4.QtPre()

defines 
{
      "SYMBIANUTILS_BUILD_LIB"
    , "JSON_BUILD_LIB"
}

dofile( solution().basedir .. "/src/shared/symbianutils/symbianutils4.lua" )

qt4.Configure()
Configure()


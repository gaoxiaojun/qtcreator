-- ----------------------------------------------------------------------------
-- Aggregation premake file.
--
-- Author:     JR Lewis <jason.lewis@gentex.com>
-- Date:       05/03/2011
-- Version:    1.0.0
-- ----------------------------------------------------------------------------

dofile (solution().basedir .. "/build/qt4.lua")

project "aggregation"
language "C++"
kind "SharedLib"

qt4.QtPre()

defines
{
    "AGGREGATION_LIBRARY"
}

files
{
      "aggregate.h"
    , "aggregation_global.h"
    , "aggregate.cpp"
}


qt4.HEADERS
{
      "aggregate.h"
    , "aggregation_global.h"
}


qt4.QT
{
    "core"
}

qt4.Configure()
Configure()

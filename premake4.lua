-- ----------------------------------------------------------------------------
-- Premake4 script to build Qt Creator.
--
-- Author:     JR Lewis <jason.lewis@gentex.com>
-- Date:       05/03/2011
-- Version:    1.0.0
-- ----------------------------------------------------------------------------

-- INCLUDES -------------------------------------------------------------------
--
dofile("build/presets4.lua")
dofile("build/qtpresets4.lua")


-- SOLUTION SETTINGS ----------------------------------------------------------
--
solution "qtcreator"
targetdir "bin"
configurations { "Release", "Debug" }

-- OPTIONS --------------------------------------------------------------------
--
EnableOption( "dynamic-runtime" )
EnableOption( "qt-shared" )

dofile("src/app/app4.lua")

-- Code to build plugins.
dofile("src/libs/3rdparty/botan/src/botan4.lua")
dofile("src/libs/utils/utils4.lua")
dofile("src/plugins/coreplugin/coreplugin4.lua")
dofile("src/plugins/find/find4.lua")
dofile("src/plugins/locator/locator4.lua")
dofile("src/plugins/texteditor/texteditor4.lua")
dofile("src/plugins/helloworld/helloworld4.lua")
dofile("src/plugins/projectexplorer/projectexplorer4.lua")
dofile("src/plugins/qtsupport/qtsupport4.lua")
dofile("src/plugins/cpptools/cpptools4.lua")
dofile("src/plugins/qmljstools/qmljstools4.lua")
dofile("src/plugins/qmljseditor/qmljseditor4.lua")

dofile("src/libs/aggregation/aggregation4.lua")
dofile("src/libs/extensionsystem/extensionsystem4.lua")
dofile("src/libs/qmljs/qmljs4.lua")
dofile("src/libs/languageutils/languageutils4.lua")
dofile("src/libs/cplusplus/cplusplus4.lua")
dofile("src/libs/qmleditorwidgets/qmleditorwidgets4.lua")

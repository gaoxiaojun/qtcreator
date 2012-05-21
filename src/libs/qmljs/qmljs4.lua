-- ----------------------------------------------------------------------------
-- QMLJS premake file.
--
-- Author:     JR Lewis <jason.lewis@gentex.com>
-- Date:       05/14/2011
-- Version:    1.0.0
-- ----------------------------------------------------------------------------
  
dofile (solution().basedir .. "/build/qt4.lua")

project "QmlJS"
language "C++"
kind "SharedLib"

qt4.QtPre()


defines 
{
      "QMLJS_BUILD_DIR"
    , "QT_CREATOR"
    , "QMLJS_BUILD_DIR"
}

qt4.QT { "core", "gui", "script" }

includedirs
{
      "."
    , ".."
}

qt4.HEADERS 
{
      "qmljs_global.h"
    , "qmljsbind.h"
    , "qmljsevaluate.h"
    , "qmljsdocument.h"
    , "qmljsscanner.h"
    , "qmljsinterpreter.h"
    , "qmljslink.h"
    , "qmljscheck.h"
    , "qmljsscopebuilder.h"
    , "qmljslineinfo.h"
    , "qmljscompletioncontextfinder.h"
    , "qmljsmodelmanagerinterface.h"
    , "qmljsicontextpane.h"
    , "qmljspropertyreader.h"
    , "qmljsrewriter.h"
    , "qmljsicons.h"
    , "qmljsdelta.h"
    , "qmljstypedescriptionreader.h"
    , "qmljsscopeastpath.h"
    , "qmljsvalueowner.h"
    , "qmljscontext.h"
    , "qmljsscopechain.h"
    , "qmljsutils.h"
    , "qmljsstaticanalysismessage.h"
    , "jsoncheck.h"
}

files
{
      "qmljsbind.cpp"
    , "qmljsevaluate.cpp"
    , "qmljsdocument.cpp"
    , "qmljsscanner.cpp"
    , "qmljsinterpreter.cpp"
    , "qmljslink.cpp"
    , "qmljscheck.cpp"
    , "qmljsscopebuilder.cpp"
    , "qmljslineinfo.cpp"
    , "qmljscompletioncontextfinder.cpp"
    , "qmljsmodelmanagerinterface.cpp"
    , "qmljspropertyreader.cpp"
    , "qmljsrewriter.cpp"
    , "qmljsicons.cpp"
    , "qmljsdelta.cpp"
    , "qmljstypedescriptionreader.cpp"
    , "qmljsscopeastpath.cpp"
    , "qmljsvalueowner.cpp"
    , "qmljscontext.cpp"
    , "qmljsscopechain.cpp"
    , "qmljsutils.cpp"
    , "qmljsstaticanalysismessage.cpp"
    , "jsoncheck.cpp"
}

qt4.RESOURCES { "qmljs.qrc" }

files
{
      "qmljsindenter.cpp"
    , "qmljscodeformatter.cpp"
    , "qmljsreformatter.cpp"
}

qt4.HEADERS
{
      "qmljsindenter.h"
    , "qmljscodeformatter.h"
    , "qmljsreformatter.h"
}

qt4.HEADERS
{
      "parser/qmljsast_p.h"
    , "parser/qmljsastfwd_p.h"
    , "parser/qmljsastvisitor_p.h"
    , "parser/qmljsengine_p.h"
    , "parser/qmljsgrammar_p.h"
    , "parser/qmljslexer_p.h"
    , "parser/qmljsmemorypool_p.h"
    , "parser/qmljsparser_p.h"
    , "parser/qmljsglobal_p.h"
    , "parser/qmldirparser_p.h"
    , "parser/qmlerror.h"
    , "parser/qmljskeywords_p.h"
}

files
{
      "parser/qmljsast.cpp"
    , "parser/qmljsastvisitor.cpp"
    , "parser/qmljsengine_p.cpp"
    , "parser/qmljsgrammar.cpp"
    , "parser/qmljslexer.cpp"
    , "parser/qmljsparser.cpp"
    , "parser/qmldirparser.cpp"
    , "parser/qmlerror.cpp"
}

links
{
      "Utils"
    , "LanguageUtils"
    , "Botan"
}

qt4.Configure()
Configure()

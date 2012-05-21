dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/libs/3rdparty/cplusplus/cplusplus4.lua")

project "CPlusPlus"
language "C++"
kind "SharedLib"

qt4.QtPre()

qt4.QT
{
      "core"
    , "gui"
}

defines 
{ 
      "NDEBUG" 
    , "CPLUSPLUS_BUILD_LIB"
} 

includedirs 
{
      ".."
    , "."
}

qt4.HEADERS
{
      "Icons.h"
    , "ExpressionUnderCursor.h"
    , "BackwardsScanner.h"
    , "MatchingText.h"
    , "OverviewModel.h"
}

files
{
      "Icons.cpp"
    , "ExpressionUnderCursor.cpp"
    , "BackwardsScanner.cpp"
    , "MatchingText.cpp"
    , "OverviewModel.cpp"
}

qt4.HEADERS
{
      "SimpleLexer.h"
    , "CppDocument.h"
    , "CppRewriter.h"
    , "Overview.h"
    , "NamePrettyPrinter.h"
    , "TypeOfExpression.h"
    , "TypePrettyPrinter.h"
    , "ResolveExpression.h"
    , "LookupItem.h"
    , "LookupContext.h"
    , "ASTParent.h"
    , "ASTPath.h"
    , "SnapshotSymbolVisitor.h"
    , "SymbolNameVisitor.h"
    , "DeprecatedGenTemplateInstance.h"
    , "FindUsages.h"
    , "DependencyTable.h"
    , "PreprocessorClient.h"
    , "PreprocessorEnvironment.h"
    , "Macro.h"
    , "FastPreprocessor.h"
    , "pp.h"
    , "pp-cctype.h"
    , "pp-engine.h"
    , "pp-scanner.h"
    , "findcdbbreakpoint.h"
    , "PPToken.h"
}


files
{
      "SimpleLexer.cpp"
    , "CppDocument.cpp"
    , "CppRewriter.cpp"
    , "Overview.cpp"
    , "NamePrettyPrinter.cpp"
    , "TypeOfExpression.cpp"
    , "TypePrettyPrinter.cpp"
    , "ResolveExpression.cpp"
    , "LookupItem.cpp"
    , "LookupContext.cpp"
    , "ASTParent.cpp"
    , "ASTPath.cpp"
    , "SnapshotSymbolVisitor.cpp"
    , "SymbolNameVisitor.cpp"
    , "DeprecatedGenTemplateInstance.cpp"
    , "FindUsages.cpp"
    , "DependencyTable.cpp"
    , "PreprocessorClient.cpp"
    , "PreprocessorEnvironment.cpp"
    , "FastPreprocessor.cpp"
    , "Macro.cpp"
    , "pp-engine.cpp"
    , "pp-scanner.cpp"
    , "findcdbbreakpoint.cpp"
    , "PPToken.cpp"
}

qt4.RESOURCES { "cplusplus.qrc" }


cplusplus.Configure()
qt4.Configure()
Configure()

--include(../languageutils/languageutils.pri)

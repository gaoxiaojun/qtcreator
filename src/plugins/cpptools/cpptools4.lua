dofile (solution().basedir .. "/build/qt4.lua")

project "CppTools"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.QT
{
      "core"
    , "gui"
}

--include($$IDE_SOURCE_TREE/src/plugins/locator/locator.pri)
--include(cpptools_dependencies.pri)

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL
{
      "CppTools.pluginspec"
    , solution().basedir .. "/bin/libCppTools*.so"
    , solution().basedir .. "/bin/CppTools*.dll"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("CppTools.pluginspec.in")

defines 
{ 
      "QT_NO_CAST_TO_ASCII" 
    , "CPPTOOLS_LIBRARY"
}


includedirs 
{
      "."
    , ".."
    , solution().basedir .. "/src"
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/libs/cplusplus"
    , solution().basedir .. "/src/libs/3rdparty/cplusplus"
    , solution().basedir .. "/src/libs/3rdparty"
}


qt4.HEADERS 
{
      "completionsettingspage.h"
    , "cppclassesfilter.h"
    , "cppcurrentdocumentfilter.h"
    , "cppfunctionsfilter.h"
    , "cppmodelmanager.h"
    , "cpplocatorfilter.h"
    , "cpptools_global.h"
    , "cpptoolsconstants.h"
    , "cpptoolseditorsupport.h"
    , "cpptoolsplugin.h"
    , "cppqtstyleindenter.h"
    , "searchsymbols.h"
    , "cppdoxygen.h"
    , "cppfilesettingspage.h"
    , "cppfindreferences.h"
    , "cppcodeformatter.h"
    , "symbolsfindfilter.h"
    , "uicodecompletionsupport.h"
    , "insertionpointlocator.h"
    , "cpprefactoringchanges.h"
    , "abstracteditorsupport.h"
    , "cppcompletionassist.h"
    , "cppcodestylesettingspage.h"
    , "cpptoolssettings.h"
    , "cppcodestylesettings.h"
    , "cppcodestylepreferencesfactory.h"
    , "cppcodestylepreferences.h"
    , "cpptoolsreuse.h"
    , "doxygengenerator.h"
    , "commentssettings.h"
    , "symbolfinder.h"
    , "cppcompletionsupport.h"
    , "cpphighlightingsupport.h"
    , "cpphighlightingsupportinternal.h"
    , "cppchecksymbols.h"
    , "cpplocalsymbols.h"
    , "cppsemanticinfo.h"
    , "cppcompletionassistprovider.h"
    , "ModelManagerInterface.h"
    , "TypeHierarchyBuilder.h"
}

files 
{
      "completionsettingspage.cpp"
    , "cppclassesfilter.cpp"
    , "cppcurrentdocumentfilter.cpp"
    , "cppfunctionsfilter.cpp"
    , "cppmodelmanager.cpp"
    , "cpplocatorfilter.cpp"
    , "cpptoolseditorsupport.cpp"
    , "cpptoolsplugin.cpp"
    , "cppqtstyleindenter.cpp"
    , "searchsymbols.cpp"
    , "cppdoxygen.cpp"
    , "cppfilesettingspage.cpp"
    , "abstracteditorsupport.cpp"
    , "cppfindreferences.cpp"
    , "cppcodeformatter.cpp"
    , "symbolsfindfilter.cpp"
    , "uicodecompletionsupport.cpp"
    , "insertionpointlocator.cpp"
    , "cpprefactoringchanges.cpp"
    , "cppcompletionassist.cpp"
    , "cppcodestylesettingspage.cpp"
    , "cpptoolssettings.cpp"
    , "cppcodestylesettings.cpp"
    , "cppcodestylepreferencesfactory.cpp"
    , "cppcodestylepreferences.cpp"
    , "cpptoolsreuse.cpp"
    , "doxygengenerator.cpp"
    , "commentssettings.cpp"
    , "symbolfinder.cpp"
    , "cppcompletionsupport.cpp"
    , "cpphighlightingsupport.cpp"
    , "cpphighlightingsupportinternal.cpp"
    , "cppchecksymbols.cpp"
    , "cpplocalsymbols.cpp"
    , "cppsemanticinfo.cpp"
    , "cppcompletionassistprovider.cpp"
    , "ModelManagerInterface.cpp"
    , "TypeHierarchyBuilder.cpp"
}

qt4.FORMS 
{
      "completionsettingspage.ui"
    , "cppfilesettingspage.ui"
    , "cppcodestylesettingspage.ui"
}

links
{
      "TextEditor"
    , "ProjectExplorer"
    , "Core"
    , "Find"
    , "Locator"
    , "Utils"
    , "CPlusPlus"
    , "extensionsystem"
    , "aggregation"
}

qt4.Configure()
Configure()


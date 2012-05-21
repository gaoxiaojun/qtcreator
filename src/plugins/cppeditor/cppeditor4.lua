dofile (solution().basedir .. "/build/qt4.lua")

project "CppEditor"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.QT 
{
      "core"
    , "gui"
}

defines
{
      "CPPEDITOR_LIBRARY"
}

includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/libs/cplusplus"
    , solution().basedir .. "/src/libs/3rdparty"
    , solution().basedir .. "/src/libs/3rdparty/cplusplus"
    , solution().basedir .. "/src/plugins"
}

qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL
{
      "CppEditor.pluginspec"
    , solution().basedir .. "/bin/libCppEditor*.so"
    , solution().basedir .. "/bin/CppEditor*.dll"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("CppEditor.pluginspec.in")

--include(cppeditor_dependencies.pri)
qt4.HEADERS 
{
      "cppplugin.h"
    , "cppeditor.h"
    , "cpphighlighter.h"
    , "cpphoverhandler.h"
    , "cppfilewizard.h"
    , "cppeditorconstants.h"
    , "cppeditorenums.h"
    , "cppeditor_global.h"
    , "cppclasswizard.h"
    , "cppoutline.h"
    , "cppinsertdecldef.h"
    , "cpptypehierarchy.h"
    , "cppelementevaluator.h"
    , "cppautocompleter.h"
    , "cppcompleteswitch.h"
    , "cppsnippetprovider.h"
    , "cppinsertqtpropertymembers.h"
    , "cppquickfixassistant.h"
    , "cppquickfix.h"
    , "cppfunctiondecldeflink.h"
    , "cppeditor.cpp"
}

files
{
      "cppplugin.cpp"
    , "cppeditor.cpp"
    , "cpphighlighter.cpp"
    , "cpphoverhandler.cpp"
    , "cppfilewizard.cpp"
    , "cppclasswizard.cpp"
    , "cppquickfixes.cpp"
    , "cppoutline.cpp"
    , "cppinsertdecldef.cpp"
    , "cpptypehierarchy.cpp"
    , "cppelementevaluator.cpp"
    , "cppautocompleter.cpp"
    , "cppcompleteswitch.cpp"
    , "cppsnippetprovider.cpp"
    , "cppinsertqtpropertymembers.cpp"
    , "cppquickfixassistant.cpp"
    , "cppquickfix.cpp"
    , "cppfunctiondecldeflink.cpp"
}

qt4.RESOURCES { "cppeditor.qrc" }

links
{
      "Core"
    , "TextEditor"
    , "Find"
    , "Locator"
    , "CppTools"
    , "ProjectExplorer"
    , "Utils"
    , "CPlusPlus"
    , "extensionsystem"
    , "aggregation"
}

qt4.Configure()
Configure()


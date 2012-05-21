dofile (solution().basedir .. "/build/qt4.lua")

project "QmlJSTools"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.CONFIG { "plugin" }

qt4.QT
{
      "core"
    , "gui"
}


qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL
{
      "QmlJSTools.pluginspec"
    , solution().basedir .. "/bin/libQmlJSTools*.so"
    , solution().basedir .. "/bin/QmlJSTools*.dll"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("QmlJSTools.pluginspec.in")

includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/libs/3rdparty"
    , solution().basedir .. "/src/libs/3rdparty/cplusplus"
    , solution().basedir .. "/src/plugins"
    , solution().basedir .. "/src/libs/qmljs"
}

--include(qmljstools_dependencies.pri)

defines
{
      "QT_NO_CAST_TO_ASCII"
    , "QMLJSTOOLS_LIBRARY"
    , "QT_CREATOR"
}

links 
{
      "QmlJS"
    , "Core"
    , "Find"
    , "Locator"
    , "TextEditor"
    , "ProjectExplorer"
    , "QtSupport"
    , "CppTools" 
    , "Utils"
    , "LanguageUtils"
    , "extensionsystem"
    , "aggregation"
}


qt4.HEADERS
{
      "qmljstools_global.h"
    , "qmljstoolsplugin.h"
    , "qmljstoolsconstants.h"
    , "qmljstoolssettings.h"
    , "qmljscodestylepreferencesfactory.h"
    , "qmljsmodelmanager.h"
    , "qmljsqtstylecodeformatter.h"
    , "qmljsrefactoringchanges.h"
    , "qmljsplugindumper.h"
    , "qmljsfunctionfilter.h"
    , "qmljslocatordata.h"
    , "qmljsindenter.h"
    , "qmljscodestylesettingspage.h"
    , "qmljsfindexportedcpptypes.h"
    , "qmljssemanticinfo.h"
}

files
{
      "qmljstoolsplugin.cpp"
    , "qmljstoolssettings.cpp"
    , "qmljscodestylepreferencesfactory.cpp"
    , "qmljsmodelmanager.cpp"
    , "qmljsqtstylecodeformatter.cpp"
    , "qmljsrefactoringchanges.cpp"
    , "qmljsplugindumper.cpp"
    , "qmljsfunctionfilter.cpp"
    , "qmljslocatordata.cpp"
    , "qmljsindenter.cpp"
    , "qmljscodestylesettingspage.cpp"
    , "qmljsfindexportedcpptypes.cpp"
    , "qmljssemanticinfo.cpp"
}

qt4.FORMS
{
      "qmljscodestylesettingspage.ui"
}

qt4.Configure()
Configure()

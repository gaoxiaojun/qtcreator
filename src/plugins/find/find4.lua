dofile (solution().basedir .. "/build/qt4.lua")

project "Find"
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

includedirs
{
      "."
    , ".."
    , solution().basedir .. "/src"
    , solution().basedir .. "/src/libs"
}

defines { "FIND_LIBRARY" }

qt4.HEADERS
{
      "findtoolwindow.h"
    , "textfindconstants.h"
    , "ifindsupport.h"
    , "ifindfilter.h"
    , "currentdocumentfind.h"
    , "basetextfind.h"
    , "find_global.h"
    , "findtoolbar.h"
    , "findplugin.h"
    , "searchresulttreeitemdelegate.h"
    , "searchresulttreeitemroles.h"
    , "searchresulttreeitems.h"
    , "searchresulttreemodel.h"
    , "searchresulttreeview.h"
    , "searchresultwindow.h"
    , "searchresultwidget.h"
    , "treeviewfind.h"
    , "searchresultwindow.cpp"
    , "ifindsupport.cpp"
    , "searchresultwidget.cpp"
}


files
{
      "findtoolwindow.cpp"
    , "currentdocumentfind.cpp"
    , "basetextfind.cpp"
    , "findtoolbar.cpp"
    , "findplugin.cpp"
    , "searchresulttreeitemdelegate.cpp"
    , "searchresulttreeitems.cpp"
    , "searchresulttreemodel.cpp"
    , "searchresulttreeview.cpp"
    , "searchresultwindow.cpp"
    , "ifindfilter.cpp"
    , "ifindsupport.cpp"
    , "searchresultwidget.cpp"
    , "treeviewfind.cpp"
}

links
{
      "Utils"
    , "Core"
    , "extensionsystem"
    , "aggregation"
}

qt4.INSTALL 
{ 
      "Find.pluginspec"
    , solution().basedir .. "/bin/libFind*.so" 
    , solution().basedir .. "/bin/Find*.dll" 
}

qt4.FORMS { "findwidget.ui", "finddialog.ui" }
qt4.RESOURCES { "find.qrc" }

substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("Find.pluginspec.in")

qt4.Configure()
Configure()




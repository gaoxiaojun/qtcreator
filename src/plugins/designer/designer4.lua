dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/shared/designerintegrationv2/designerintegrationv24.lua")

project "Designer"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.QT 
{
      "core"
    , "designer"
    , "designercomponents"
    , "gui"
    , "xml"
}


defines
{
      "DESIGNER_LIBRARY"
    , "QT_NO_CAST_FROM_ASCII"
    , "CPP_ENABLED"
}

--include(designer_dependencies.pri)

includedirs
{
      "../../tools/utils"
    , "."
    , "cpp"
    , ".."
    , solution().basedir .. "/src/libs"
    , solution().basedir .. "/src/libs/3rdparty"
    , solution().basedir .. "/src/libs/3rdparty/cplusplus"
    , solution().basedir .. "/src/libs/cplusplus"
    , solution().basedir .. "/src/plugins"
}


qt4.INSTALL_DIRECTORY (solution().basedir .. "/" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "/qtcreator/plugins")
qt4.INSTALL
{
      "Designer.pluginspec"
    , solution().basedir .. "/bin/libDesigner*.so"
    , solution().basedir .. "/bin/Designer*.dll"
}


substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("Designer.pluginspec.in")


qt4.HEADERS 
{
      "formeditorplugin.h"
    , "formeditorfactory.h"
    , "formwindoweditor.h"
    , "formwindowfile.h"
    , "formwizard.h"
    , "qtcreatorintegration.h"
    , "designerconstants.h"
    , "settingspage.h"
    , "editorwidget.h"
    , "formeditorw.h"
    , "settingsmanager.h"
    , "formtemplatewizardpage.h"
    , "formwizarddialog.h"
    , "codemodelhelpers.h"
    , "designer_export.h"
    , "designerxmleditor.h"
    , "designercontext.h"
    , "formeditorstack.h"
    , "editordata.h"
    , "resourcehandler.h"
    , "qtdesignerformclasscodegenerator.h"
}

files
{
      "formeditorplugin.cpp"
    , "formeditorfactory.cpp"
    , "formwindoweditor.cpp"
    , "formwindowfile.cpp"
    , "formwizard.cpp"
    , "qtcreatorintegration.cpp"
    , "settingspage.cpp"
    , "editorwidget.cpp"
    , "formeditorw.cpp"
    , "settingsmanager.cpp"
    , "formtemplatewizardpage.cpp"
    , "formwizarddialog.cpp"
    , "codemodelhelpers.cpp"
    , "designerxmleditor.cpp"
    , "designercontext.cpp"
    , "formeditorstack.cpp"
    , "resourcehandler.cpp"
    , "qtdesignerformclasscodegenerator.cpp"
}

qt4.RESOURCES { "designer.qrc" }

links
{
      "Core"
    , "CppTools"
    , "TextEditor"
    , "CppEditor"
    , "Find"
    , "Locator"
    , "Utils"
    , "extensionsystem"
    , "aggregation"
}

files { "Designer.mimetypes.xml", "README.txt" }

qt4.HEADERS
{
      "cpp/formclasswizardpage.h"
    , "cpp/formclasswizarddialog.h"
    , "cpp/formclasswizard.h"
    , "cpp/formclasswizardparameters.h"
    , "cpp/cppsettingspage.h"
}

files
{
      "cpp/formclasswizardpage.cpp"
    , "cpp/formclasswizarddialog.cpp "
    , "cpp/formclasswizard.cpp"
    , "cpp/formclasswizardparameters.cpp"
    , "cpp/cppsettingspage.cpp"
}

qt4.FORMS
{
      "cpp/formclasswizardpage.ui"
    , "cpp/cppsettingspagewidget.ui"
}

designerintegrationv2.Configure()
qt4.Configure()
Configure()


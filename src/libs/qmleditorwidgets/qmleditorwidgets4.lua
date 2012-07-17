dofile (solution().basedir .. "/build/qt4.lua")

project "QmlEditorWidgets"
kind "SharedLib"
language "C++"

qt4.QtPre()

qt4.QT
{
      "core"
    , "declarative"
    , "gui"
    , "script"
}

includedirs
{
      "."
    , ".."
    , "easingpane"
}

defines
{
      "QWEAKPOINTER_ENABLE_ARROW"
    , "BUILD_QMLEDITORWIDGETS_LIB"
    , "QT_CREATOR"
}

qt4.RESOURCES
{
      "resources.qrc"
    , "easingpane/easingpane.qrc"
}

qt4.FORMS
{
    "easingpane/easingcontextpane.ui"
}

qt4.HEADERS
{
      "fontsizespinbox.h"
    , "filewidget.h"
    , "contextpanewidgetrectangle.h"
    , "contextpanewidgetimage.h"
    , "contextpanewidget.h"
    , "contextpanetextwidget.h"
    , "colorwidgets.h"
    , "colorbutton.h"
    , "colorbox.h"
    , "customcolordialog.h"
    , "gradientline.h"
    , "huecontrol.h"
    , "qmleditorwidgets_global.h"
}


qt4.HEADERS
{
      "easingpane/easinggraph.h"
    , "easingpane/easingcontextpane.h"
    , "easingpane/easingcontextpane.cpp"
}


files
{
      "fontsizespinbox.cpp"
    , "filewidget.cpp"
    , "contextpanewidgetrectangle.cpp"
    , "contextpanewidgetimage.cpp"
    , "contextpanewidget.cpp"
    , "contextpanetextwidget.cpp"
    , "colorwidgets.cpp"
    , "colorbox.cpp"
    , "customcolordialog.cpp"
    , "huecontrol.cpp"
    , "gradientline.cpp"
    , "colorbutton.cpp"
    , "easingpane/easinggraph.cpp"
    , "easingpane/easingcontextpane.cpp"
}

qt4.FORMS
{
      "contextpanewidgetrectangle.ui"
    , "contextpanewidgetimage.ui"
    , "contextpanewidgetborderimage.ui"
    , "contextpanetext.ui"
}

links
{
	"QmlJS"
}

qt4.Configure()
Configure()


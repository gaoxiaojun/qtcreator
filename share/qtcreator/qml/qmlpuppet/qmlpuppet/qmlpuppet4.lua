dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/share/qtcreator/qml/qmlpuppet/commands/puppet_commands4.lua")
dofile (solution().basedir .. "/share/qtcreator/qml/qmlpuppet/container/puppet_container4.lua")
dofile (solution().basedir .. "/share/qtcreator/qml/qmlpuppet/interfaces/puppet_interfaces4.lua")
dofile (solution().basedir .. "/share/qtcreator/qml/qmlpuppet/instances/puppet_instances4.lua")
dofile (solution().basedir .. "/share/qtcreator/qml/qmlpuppet/qmlpuppet/instances/instances4.lua")

project "qmlpuppet"
kind "WindowedApp"
language "C++"

qt4.QtPre()

qt4.QT { "core", "gui", "declarative", "network", "webkit" }

defines { "QWEAKPOINTER_ENABLE_ARROW" }

files
{
    "main.cpp"
}

qt4.RESOURCES
{
    solution().basedir .. "/share/qtcreator/qml/qmlpuppet/qmlpuppet.qrc"
}

instances4.Configure()
puppet_commands4.Configure()
puppet_container4.Configure()
puppet_interfaces4.Configure()
puppet_instances4.Configure()
qt4.Configure()
Configure()


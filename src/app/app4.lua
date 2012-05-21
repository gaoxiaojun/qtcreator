dofile (solution().basedir .. "/build/qt4.lua")
dofile (solution().basedir .. "/src/shared/qtsingleapplication/qtsingleapplication4.lua")

project "QtCreator"
kind "WindowedApp"
language "C++"

qt4.QtPre()

qtsingleapplication.Configure()

qt4.CopyLibraries(true)

-- Generate the app_version.h file.
substitutes.AddFakeFunction("replace", 2)
substitutes.AddFakeFunction("replace", 3)
substitutes.AddFakeFunction("replace", 1)
substitutes.Add("QTCREATOR_VERSION", "2.3.1")
qt4.SUBSTITUTE("app_version.h.in")

includedirs
{
      solution().basedir .. "/src/libs/extensionsystem"
    , solution().basedir .. "/src/libs/aggregation"
    , solution().basedir .. "/src/libs"
    , ".."
}

qt4.QT
{
      "gui"
    , "core"
}

if (os.is("linux")) then
    defines { "IDE_LIBRARY_BASENAME=\"\\\"" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "\\\"\"" }
elseif (os.is("windows")) then
    defines { "IDE_LIBRARY_BASENAME=\"" .. os.getenv("QT_PLUGINS_DIRECTORY") .. "\"" }
else
    print("Unrecognized OS type: " .. os.get() .. " in app4.lua")
end

files
{
    "main.cpp"
}

links
{
    "aggregation", "extensionsystem"
}

if (os.is("linux")) then
    _OPTIONS["rpath"] = "$$``ORIGIN,-rpath," .. qt4.private.lib_dir
end

qt4.Configure()
Configure()


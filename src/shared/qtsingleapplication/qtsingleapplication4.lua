dofile (solution().basedir .. "/src/shared/qtlockedfile/qtlockedfile4.lua")

qtsingleapplication = { }

function qtsingleapplication.Configure()

    qtsingleapplication.directory = solution().basedir .. "/src/shared/qtsingleapplication"

    includedirs
    {
        qtsingleapplication.directory 
    }
    
    
    qt4.QT
    {
        "Network"
    }
    
    
    qt4.HEADERS
    {
          qtsingleapplication.directory .. "/qtsingleapplication.h"    
        , qtsingleapplication.directory .. "/qtlocalpeer.h" 
    }
    
    files
    {
          qtsingleapplication.directory .. "/qtsingleapplication.cpp"    
        , qtsingleapplication.directory .. "/qtlocalpeer.cpp" 
    }
    
    qtlockedfile.Configure()
    
    if (os.is("windows")) then
        defines
        {
            "QT_QTSINGLEAPPLICATION_EXPORT=__declspec\\(dllexport\\)"
        }
    end

end

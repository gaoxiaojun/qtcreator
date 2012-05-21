qtlockedfile = { }

function qtlockedfile.Configure()
    qtlockedfile.directory = solution().basedir .. "/src/shared/qtlockedfile"
    
    includedirs
    {
        qtlockedfile.directory
    }
    
    qt4.HEADERS
    {
        qtlockedfile.directory .. "/qtlockedfile.h"
    }
    
    files
    {
        qtlockedfile.directory .. "/qtlockedfile.cpp"
    }
    
    if (os.is("linux")) then
        files
        {
            qtlockedfile.directory .. "/qtlockedfile_unix.cpp"
        }
    elseif (os.is("windows")) then
        files
        {
            qtlockedfile.directory .. "/qtlockedfile_win.cpp"
        }
    end
    
    if (os.is("windows")) then
        defines
        {
            "QT_QTLOCKEDFILE_EXPORT=__declspec(dllexport)"
        }
    end
end


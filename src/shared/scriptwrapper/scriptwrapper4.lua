scriptwrapper = { }

function scriptwrapper.Configure()
    local d = solution().basedir .. "/src/shared/scriptwrapper"
    includedirs 
    {
        d
    }

    qt4.HEADERS
    {
        d .. "/wrap_helpers.h"
    }
end


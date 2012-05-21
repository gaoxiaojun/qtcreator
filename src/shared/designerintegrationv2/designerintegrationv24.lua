designerintegrationv2 = { }

function designerintegrationv2.Configure()
    local d = solution().basedir .. "/src/shared/designerintegrationv2"
    includedirs
    {
        d, d .."/.."
    }

    files 
    {
          d .. "/widgethost.cpp"
        , d .. "/sizehandlerect.cpp"
        , d .. "/formresizer.cpp"
    }

    qt4.HEADERS 
    {
          d .. "/widgethost.h"
        , d .. "/sizehandlerect.h"
        , d .. "/formresizer.h"
        , d .. "/widgethostconstants.h"
    }

end


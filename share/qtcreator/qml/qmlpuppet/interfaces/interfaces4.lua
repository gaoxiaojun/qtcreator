interfaces4 = {}

function interfaces4.Configure()

    local d = solution().basedir .. "/share/qtcreator/qml/qmlpuppet/interfaces"

    includedirs
    {
        d 
    }

    qt4.HEADERS
    {
          d .. "/nodeinstanceclientinterface.h"
        , d .. "/nodeinstanceserverinterface.h"
        , d .. "/commondefines.h"
    }

    files
    {
        d .. "/nodeinstancesserverinterface.cpp"
    }

end


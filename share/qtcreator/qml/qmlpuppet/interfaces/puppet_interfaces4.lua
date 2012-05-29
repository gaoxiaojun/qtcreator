puppet_interfaces4 = {}

function puppet_interfaces4.Configure()

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
        , d .. "/nodeinstanceserverinterface.cpp"
    }

    files
    {
        d .. "/nodeinstanceserverinterface.cpp"
    }

end


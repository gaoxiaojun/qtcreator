puppet_container4 = {}

function puppet_container4.Configure()

    local d = solution().basedir .. "/share/qtcreator/qml/qmlpuppet/container"

    includedirs
    {
          d 
    }


    qt4.HEADERS
    {
          d .. "/addimportcontainer.h"
        , d .. "/imagecontainer.h"
        , d .. "/idcontainer.h"
        , d .. "/informationcontainer.h"
        , d .. "/instancecontainer.h"
        , d .. "/reparentcontainer.h"
        , d .. "/propertyabstractcontainer.h"
        , d .. "/propertybindingcontainer.h"
        , d .. "/propertyvaluecontainer.h"
    }

    files
    {
          d .. "/addimportcontainer.cpp"
        , d .. "/imagecontainer.cpp"
        , d .. "/idcontainer.cpp"
        , d .. "/informationcontainer.cpp"
        , d .. "/instancecontainer.cpp"
        , d .. "/reparentcontainer.cpp"
        , d .. "/propertyabstractcontainer.cpp"
        , d .. "/propertybindingcontainer.cpp"
        , d .. "/propertyvaluecontainer.cpp"
    }

end


instances4 = {}

function instances4.Configure()

    local d = solution().basedir .. "/share/qtcreator/qml/qmlpuppet/qmlpuppet/instances"

    includedirs
    {
        d
    }


    qt4.HEADERS
    {
          d .. "/qt4nodeinstanceserver.h"
        , d .. "/qt4nodeinstanceclientproxy.h"
        , d .. "/qt4informationnodeinstanceserver.h"
        , d .. "/qt4rendernodeinstanceserver.h"
        , d .. "/qt4previewnodeinstanceserver.h"
        , d .. "/graphicsobjectnodeinstance.h"
        , d .. "/positionernodeinstance.h"
        , d .. "/qmlgraphicsitemnodeinstance.h"
    }


    files
    {
          d .. "/qt4nodeinstanceserver.cpp"
        , d .. "/qt4nodeinstanceclientproxy.cpp"
        , d .. "/qt4informationnodeinstanceserver.cpp"
        , d .. "/qt4rendernodeinstanceserver.cpp"
        , d .. "/qt4previewnodeinstanceserver.cpp"
        , d .. "/graphicsobjectnodeinstance.cpp"
        , d .. "/qmlgraphicsitemnodeinstance.cpp"
        , d .. "/positionernodeinstance.cpp"
    }

end


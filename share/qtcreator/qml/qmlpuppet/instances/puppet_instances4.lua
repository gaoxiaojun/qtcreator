puppet_instances4 = {}

function puppet_instances4.Configure()

    local d = solution().basedir .. "/share/qtcreator/qml/qmlpuppet/instances"

    includedirs
    {
          d 
    }

    qt4.HEADERS
    {
          d .. "/behaviornodeinstance.h"
        , d .. "/dummycontextobject.h"
        , d .. "/childrenchangeeventfilter.h"
        , d .. "/componentnodeinstance.h"
        , d .. "/dummynodeinstance.h"
        , d .. "/nodeinstanceclientproxy.h"
        , d .. "/nodeinstancemetaobject.h"
        , d .. "/nodeinstanceserver.h"
        , d .. "/nodeinstancesignalspy.h"
        , d .. "/objectnodeinstance.h"
        , d .. "/qmlpropertychangesnodeinstance.h"
        , d .. "/qmlstatenodeinstance.h"
        , d .. "/qmltransitionnodeinstance.h"
        , d .. "/servernodeinstance.h"
        , d .. "/anchorchangesnodeinstance.h"
    }


    files
    {
          d .. "/behaviornodeinstance.cpp"
        , d .. "/dummycontextobject.cpp"
        , d .. "/childrenchangeeventfilter.cpp"
        , d .. "/componentnodeinstance.cpp"
        , d .. "/dummynodeinstance.cpp"
        , d .. "/nodeinstanceclientproxy.cpp"
        , d .. "/nodeinstancemetaobject.cpp"
        , d .. "/nodeinstanceserver.cpp"
        , d .. "/nodeinstancesignalspy.cpp"
        , d .. "/objectnodeinstance.cpp"
        , d .. "/qmlpropertychangesnodeinstance.cpp"
        , d .. "/qmlstatenodeinstance.cpp"
        , d .. "/qmltransitionnodeinstance.cpp"
        , d .. "/servernodeinstance.cpp"
        , d .. "/anchorchangesnodeinstance.cpp"
    }

end


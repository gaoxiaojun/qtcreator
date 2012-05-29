puppet_commands4 = {}

function puppet_commands4.Configure()

    local d = solution().basedir .. "/share/qtcreator/qml/qmlpuppet/commands"
    local base = solution().basedir .. "/share/qtcreator/qml/qmlpuppet"

    includedirs
    {
          d 
    }

    qt4.HEADERS
    {
          d .. "/synchronizecommand.h"
        , d .. "/tokencommand.h"
        , d .. "/componentcompletedcommand.h"
        , d .. "/completecomponentcommand.h"
        , d .. "/statepreviewimagechangedcommand.h"
        , d .. "/childrenchangedcommand.h"
        , d .. "/changebindingscommand.h"
        , d .. "/changefileurlcommand.h"
        , d .. "/changeidscommand.h"
        , d .. "/changenodesourcecommand.h"
        , d .. "/changestatecommand.h"
        , d .. "/changevaluescommand.h"
        , d .. "/createscenecommand.h"
        , d .. "/clearscenecommand.h"
        , d .. "/createinstancescommand.h"
        , d .. "/informationchangedcommand.h"
        , d .. "/pixmapchangedcommand.h"
        , d .. "/removeinstancescommand.h"
        , d .. "/removepropertiescommand.h"
        , d .. "/reparentinstancescommand.h"
        , d .. "/valueschangedcommand.h"
        , d .. "/changeauxiliarycommand.h"
    }

    files
    {
          d .. "/synchronizecommand.cpp"
        , d .. "/tokencommand.cpp"
        , d .. "/componentcompletedcommand.cpp"
        , d .. "/completecomponentcommand.cpp"
        , d .. "/statepreviewimagechangedcommand.cpp"
        , d .. "/childrenchangedcommand.cpp"
        , d .. "/changebindingscommand.cpp"
        , d .. "/changefileurlcommand.cpp"
        , d .. "/changeidscommand.cpp"
        , d .. "/changenodesourcecommand.cpp"
        , d .. "/changestatecommand.cpp"
        , d .. "/changevaluescommand.cpp"
        , d .. "/informationchangedcommand.cpp"
        , d .. "/removeinstancescommand.cpp"
        , d .. "/removepropertiescommand.cpp"
        , d .. "/reparentinstancescommand.cpp"
        , d .. "/valueschangedcommand.cpp"
        , d .. "/clearscenecommand.cpp"
        , d .. "/createinstancescommand.cpp"
        , d .. "/createscenecommand.cpp"
        , d .. "/pixmapchangedcommand.cpp"
        , d .. "/changeauxiliarycommand.cpp"
    }

end


proparser = { }

function proparser.Configure()

    local d = solution().basedir .. "/src/shared/proparser"

    qt4.QT { "xml" }

    includedirs
    {
        d
    }

    qt4.HEADERS
    {
          d .. "/proparser_global.h"
        , d .. "/profileparser.h"
        , d .. "/profileevaluator.h"
        , d .. "/proitems.h"
        , d .. "/prowriter.h"
        , d .. "/ioutils.h"
    }

    files
    {
          d .. "/profileparser.cpp"
        , d .. "/profileevaluator.cpp"
        , d .. "/proitems.cpp"
        , d .. "/prowriter.cpp"
        , d .. "/ioutils.cpp"
    }

    qt4.RESOURCES { d .. "/proparser.qrc" }

end

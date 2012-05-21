private_headers = { }

function private_headers.Configure()
    local d = os.getenv("QT_PRIVATE_HEADERS")

    if (d == nil) then
        if (os.getenv("QTDIR")) then
            d = os.getenv("QTDIR") .. "/include"
        end
    end

    if (not d) then
        error("private_headers4.lua fails, define QT_PRIVATE_HEADERS or QTDIR.")
    end

    includedirs
    {
          d
        , d .. "/QtCore"
        , d .. "/QtGui"
        , d .. "/QtScript"
        , d .. "/QtDeclarative" 
    }
end


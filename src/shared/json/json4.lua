
json = { }
json.directory = solution().basedir .. "/src/shared/json"

includedirs 
{
   json.directory
}

qt4.HEADERS
{
      json.directory .. "/json_global.h"
    , json.directory .. "/json.h" 
}



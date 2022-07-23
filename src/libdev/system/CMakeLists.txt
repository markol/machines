
set(SYSTEM_SOURCES
    filedata.cpp
    filedata.hpp
    filedate.cpp
    filedate.hpp
    fileenum.cpp
    fileenum.hpp
    filefind.cpp
    filefind.hpp
    filetime.cpp
    filetime.hpp
    memcaps.cpp
    memcaps.hpp
    metafile.cpp
    metafile.hpp
    metaistr.cpp
    metaistr.hpp
    pathname.cpp
    pathname.hpp
    registry.cpp
    registry.hpp
    winapi.cpp
    winapi.hpp
)

add_library(system OBJECT ${SYSTEM_SOURCES})


set(ENVIRNMT_SOURCES
    elevclut.cpp
    elevclut.hpp
    elevmatp.cpp
    elevmatp.hpp
    internal/orbparam.hpp
    internal/plaparse.hpp
    internal/satparam.hpp
    internal/skydecl.hpp
    lexer.cpp
    orbit.cpp
    orbit.hpp
    orbparam.cpp
    parser.cpp
    planet.cpp
    planet.hpp
    plaparse.cpp
    satelite.cpp
    satelite.hpp
    satparam.cpp
    sky.cpp
    skydecl.cpp
    sky.hpp
    sky.ipp
    skyline.cpp
    skyline.hpp
)

add_library(envirnmt OBJECT ${ENVIRNMT_SOURCES})

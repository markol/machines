
set(UTILITY_SOURCES
    DependencyProvider.hpp
    ascpict.cpp
    ascpict.hpp
    ascpicti.cpp
    bidpmap.hpp
    bidpmap.itp
    bidvoidm.cpp
    bndidgen.cpp
    bndidgen.hpp
    constrai.hpp
    factory.hpp
    id.hpp
    indent.hpp
    internal/ascpicti.hpp
    interval.hpp
    linetok.cpp
    linetok.hpp
    percent.hpp
    private/bidvoidm.hpp
    private/uint64.hpp
    property.hpp
    rapidxml.hpp
    rapidxml_ext.hpp
    rapidxml_print.hpp
    rapidxml_utils.hpp
    subrange.hpp
    time.hpp
    tostring.cpp
    tostring.hpp
)

add_library(utility OBJECT ${UTILITY_SOURCES})

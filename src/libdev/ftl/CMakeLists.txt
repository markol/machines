
set(FTL_SOURCES
    collect.hpp
    contain.hpp
    ftl.hpp
    map.hpp
    rbtree.hpp
    reuidgen.hpp
    serialid.hpp
    seriamap.ctp
    seriamap.hpp
    seriamap.itp
    seripmap.ctp
    seripmap.hpp
    seripmap.itp
)

add_library(ftl INTERFACE ${FTL_SOURCES})

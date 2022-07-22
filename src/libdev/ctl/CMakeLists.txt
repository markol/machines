
file(GLOB CTL_SOURCES
    algorith.hpp
    bitvec.cpp
    bitvec.hpp
    countptr.cpp
    countptr.hpp
    deque.hpp
    erase.hpp
    fixedvec.hpp
    function.hpp
    list.hpp
    map.hpp
    mmvector.hpp
    nbvector.hpp
    pvector.hpp
    queue.hpp
    set.hpp
    utility.hpp
    vector.hpp
)

add_library(ctl OBJECT ${CTL_SOURCES})

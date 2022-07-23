
set(STDLIB_SOURCES
    algorith.hpp
    bits.hpp
    bitstrin.hpp
    deque.hpp
    exceptio.cpp
    exceptio.hpp
    function.hpp
    iterator.hpp
    limits.hpp
    list.hpp
    map.hpp
    memory.hpp
    numeric.hpp
    queue.hpp
    set.hpp
    stack.hpp
    strfwd.hpp
    string.cpp
    string.hpp
    tree.hpp
    utility.hpp
    vector.hpp
)

add_library(stdlib OBJECT ${STDLIB_SOURCES})

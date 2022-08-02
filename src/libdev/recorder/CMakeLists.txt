
set(RECORDER_SOURCES
    internal/recimpl.hpp
    private/recpriv.ctf
    private/recpriv.hpp
    private/recpriv.itf
    recimpl.cpp
    recorder.cpp
    recorder.hpp
    recpriv.cpp
)

add_library(recorder OBJECT ${RECORDER_SOURCES})

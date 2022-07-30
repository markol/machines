
set(LIBAUDIO_SOURCES
    fileutil.c
    wave.c
)

add_library(libaudio STATIC ${LIBAUDIO_SOURCES})

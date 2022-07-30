
set(LIBSMACKER_SOURCES
    smacker.c
    smk_bitstream.c
    smk_hufftree.c
)

add_library(libsmacker OBJECT ${LIBSMACKER_SOURCES})

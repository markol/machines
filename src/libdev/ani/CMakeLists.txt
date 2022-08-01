
set(ANI_SOURCES
    AniSmackerCutscene.cpp
    AniSmackerCutscene.hpp
    AniSmacker.hpp
    AniSmackerRegular.cpp
    AniSmackerRegular.hpp
    FrameScaler.cpp
    FrameScaler.hpp
)

add_library(ani OBJECT ${ANI_SOURCES})

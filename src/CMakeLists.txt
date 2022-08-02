# Compile flags as defined in global CMakeLists
set(CMAKE_CXX_FLAGS "${MACHINES_CXX_FLAGS} ${MXE_CFLAGS}")
set(CMAKE_C_FLAGS "${MACHINES_C_FLAGS} ${MXE_CFLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE ${MACHINES_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEBUG ${MACHINES_CXX_FLAGS_DEBUG})

# Subdirectories

#add_subdirectory(libdev)
#add_subdirectory(projects)



# Source files
# GLOB is sometimes considered anti pattern but watever

set(MACHINES_SOURCES
    projects/machines/sdlapp.cpp
    projects/machines/loadplan.cpp
    projects/machines/sndstuff.cpp
    projects/machines/guistuff.cpp
    projects/machines/motion.cpp
    projects/machines/leaktrak.cpp
    projects/machines/errorhnd.cpp
    projects/machines/scrndump.cpp
    projects/machines/simstuff.cpp
    #projects/machines/ti.cpp
    #projects/machines/tiobjdpo.cpp
)

file(GLOB PLANETED_SOURCES projects/planeted/*.cpp)
list(REMOVE_ITEM PLANETED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/projects/planeted/madata.cpp)
list(REMOVE_ITEM PLANETED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/projects/planeted/parmdata.cpp)
list(REMOVE_ITEM PLANETED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/projects/planeted/tiobjdpo.cpp)
list(REMOVE_ITEM PLANETED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/projects/planeted/ti.cpp)

if(PLATFORM_WINDOWS)
  set(MACHINES_SOURCES
      ${MACHINES_SOURCES}
      # Exe icon and meta info
      ${PROJECT_SOURCE_DIR}/windows/resource.rc
  )
  set(PLANETED_SOURCES
      ${PLANETED_SOURCES}
      ${PROJECT_SOURCE_DIR}/windows/planeted.rc
  )
endif()

# Libraries
set(LIBS
    ${LIBS}
    #pthread
    #SDL2
    ${SDL2_LIBRARY}
    #GL
    ${OPENGL_LIBRARY}
    #GLEW
    ${GLEW_LIBRARY}
    #SDL2_image
    ${SDL2_IMAGE_LIBRARY}
    #alure-static
    ${ALURE_LIBRARY}
    #openal
    ${OPENAL_LIBRARY}
    #audio
    #dl
    #static-libstdc++
    #freetype
    ${FREETYPE_LIBRARY}
    #enet
    ${ENET_LIBRARY}
    #swscale
    ${SWSCALE_LIBRARY}
)

set(MACHINES_LIBS ${LIBS} PARENT_SCOPE)


# Includes
set(LOCAL_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/libdev
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/projects
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/libsmack
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
    ${CMAKE_CURRENT_SOURCE_DIR}/libdev
)

set(SYSTEM_INCLUDES
    /usr/include/freetype2
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${LIBSNDFILE_INCLUDE_DIR}
    ${ALURE_INCLUDE_DIR}
    ${ENET_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
    ${SWSCALE_INCLUDE_DIR}
)


include_directories(${LOCAL_INCLUDES})
include_directories(SYSTEM ${SYSTEM_INCLUDES})


# Link directories
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/.
)


# Targets

add_subdirectory(libdev/afx)
add_subdirectory(libdev/base)
add_subdirectory(libdev/ctl)
add_subdirectory(libdev/ftl)
add_subdirectory(libdev/mathex)
add_subdirectory(libdev/gxin)
add_subdirectory(libdev/xin)
add_subdirectory(libdev/phys)
add_subdirectory(libdev/stdlib)
add_subdirectory(libdev/profiler)
add_subdirectory(libdev/system)
add_subdirectory(libdev/device)
add_subdirectory(libdev/utility)
add_subdirectory(libdev/sound)
add_subdirectory(libdev/recorder)
add_subdirectory(libdev/world4d)
add_subdirectory(libdev/network)
add_subdirectory(libdev/render)
add_subdirectory(libdev/gui)
add_subdirectory(libdev/sim)
add_subdirectory(libdev/machphys)
add_subdirectory(libdev/envirnmt)
add_subdirectory(libdev/ani)
add_subdirectory(libdev/machlog)
add_subdirectory(libdev/machgui)

add_library(libs INTERFACE)
target_sources(libs

    INTERFACE 
    $<TARGET_OBJECTS:base>
    $<TARGET_OBJECTS:mathex>
    $<TARGET_OBJECTS:afx>
    $<TARGET_OBJECTS:utility>
    $<TARGET_OBJECTS:profiler>
    $<TARGET_OBJECTS:system>
    $<TARGET_OBJECTS:device>
    $<TARGET_OBJECTS:world4d>
    $<TARGET_OBJECTS:gxin>
    $<TARGET_OBJECTS:xin>
    $<TARGET_OBJECTS:stdlib>
    $<TARGET_OBJECTS:render>
    $<TARGET_OBJECTS:sound>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:ctl>
    $<TARGET_OBJECTS:phys>
    $<TARGET_OBJECTS:sim>
    $<TARGET_OBJECTS:gui>
    $<TARGET_OBJECTS:recorder>
    $<TARGET_OBJECTS:envirnmt>
    $<TARGET_OBJECTS:machphys>
    $<TARGET_OBJECTS:machlog>

)

add_library(libs_mach_gui INTERFACE)
target_sources(libs_mach_gui 

    INTERFACE 
    $<TARGET_OBJECTS:ani>
    $<TARGET_OBJECTS:machgui>
    $<TARGET_OBJECTS:libsmacker>

)

add_executable(machines ${MACHINES_SOURCES})
add_executable(planeted ${PLANETED_SOURCES})
set_target_properties(machines planeted PROPERTIES LINK_FLAGS_RELEASE -s)
set_target_properties(machines planeted PROPERTIES 
    COMPILE_FLAGS "${MACHINES_COMPILER_BIT_MODE}" LINK_FLAGS "${MACHINES_COMPILER_BIT_MODE}"
)

target_link_libraries(machines -static-libgcc -static-libstdc++)
target_link_libraries(planeted -static-libgcc -static-libstdc++)
target_link_libraries(machines libs libs_mach_gui libaudio ${LIBS} )
target_link_libraries(planeted libs libaudio ${LIBS} )

target_link_libraries (machines -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)
target_link_libraries (planeted -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)



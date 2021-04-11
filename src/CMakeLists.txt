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
file(GLOB AFX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/afx/*.cpp)

file(GLOB BASE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/base/*.cpp)
list(REMOVE_ITEM BASE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/base/basver.cpp)
list(REMOVE_ITEM BASE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/base/test.cpp)
list(REMOVE_ITEM BASE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/base/ti1.cpp)

file(GLOB UTILITY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/utility/*.cpp)
file(GLOB PROFILER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/profiler/*.cpp)
file(GLOB SYSTEM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/system/*.cpp)

file(GLOB DEVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/device/*.cpp)
list(REMOVE_ITEM DEVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/device/commouse.cpp)

file(GLOB WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/*.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti1.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti2.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti3.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti4.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti5.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti6.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti7.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti8.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti9.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti10.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti11.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti12.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti13.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti_comp.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ticouptr.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti_penty.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti_pplan.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti_set.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/ti_smgr.cpp)
list(REMOVE_ITEM WORLD4D_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/world4d/w4dver.cpp)

file(GLOB GXIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/gxin/*.cpp)
list(REMOVE_ITEM GXIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/gxin/gxerror.cpp)

file(GLOB XIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/xin/*.cpp)
file(GLOB STDLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/stdlib/*.cpp)

file(GLOB RENDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/render/*.cpp)
list(REMOVE_ITEM RENDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/render/devidi.cpp)
list(REMOVE_ITEM RENDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/render/renerror.cpp)
list(REMOVE_ITEM RENDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/render/backgrnd.cpp)
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/libdev/render/mesh.cpp PROPERTIES COMPILE_FLAGS -O0)

file(GLOB SOUND_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/sound/*.cpp)

file(GLOB NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/network/*.cpp)
list(REMOVE_ITEM NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/network/node.cpp)
list(REMOVE_ITEM NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/network/nodei.cpp)
list(REMOVE_ITEM NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/network/session.cpp)
list(REMOVE_ITEM NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/network/sessioni.cpp)

file(GLOB CTL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/ctl/*.cpp)

file(GLOB PHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/phys/*.cpp)
list(REMOVE_ITEM PHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/phys/ti1.cpp)
list(REMOVE_ITEM PHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/phys/ti_coptr.cpp)
list(REMOVE_ITEM PHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/phys/physver.cpp)

file(GLOB SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/sim/*.cpp)
list(REMOVE_ITEM SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/sim/tisim.cpp)

file(GLOB MACHGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machgui/*.cpp)
list(REMOVE_ITEM MACHGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machgui/tigui.cpp)
list(REMOVE_ITEM MACHGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machgui/tigui2.cpp)
list(REMOVE_ITEM MACHGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machgui/tigui3.cpp)
list(REMOVE_ITEM MACHGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machgui/tigui4.cpp)

file(GLOB GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/gui/*.cpp)
list(REMOVE_ITEM GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/gui/ti1.cpp)
list(REMOVE_ITEM GUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/gui/guiver.cpp)

file(GLOB_RECURSE MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/**.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/tt.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/scorch.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/beeblast.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/idebris.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/swlab.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/weaplab.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/mapver.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/test.cpp)
# Exclude unnecessary template instantions
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_data.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/tipch.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_cdata.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ticomp.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_links.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_data2.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_data2.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_data2.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_face.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti_sw.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti1.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti2.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti3.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti4.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti5.cpp)
list(REMOVE_ITEM MACHPHYS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machphys/ti_files/ti13.cpp)

file(GLOB ANI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/ani/*.cpp)
list(REMOVE_ITEM ANI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/ani/smackmock.cpp)

file(GLOB RECORDER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/recorder/*.cpp)

file(GLOB ENVIRNMT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/envirnmt/*.cpp)
list(REMOVE_ITEM ENVIRNMT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/envirnmt/ti_map1.cpp)
list(REMOVE_ITEM ENVIRNMT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/envirnmt/ti_map2.cpp)
list(REMOVE_ITEM ENVIRNMT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/envirnmt/ti_clut.cpp)

file(GLOB MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/*.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti1.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti2.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti3.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti4.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti5.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti6.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti7.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/ti8.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/tipch.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/reductio.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/consdata.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/opconsti.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/machdesc.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/oplandoms.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/swlab.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/dbgstuff.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/pchml.cpp)
list(REMOVE_ITEM MACHLOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/malver.cpp)
#set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/libdev/machlog/persist.cpp PROPERTIES COMPILE_FLAGS -O0)

file(GLOB MATHEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/mathex/*.cpp)
list(REMOVE_ITEM MATHEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/mathex/ti.cpp)
list(REMOVE_ITEM MATHEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/mathex/invxform.cpp)
list(REMOVE_ITEM MATHEX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libdev/mathex/mexver.cpp)


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

# TODO: move to another subdirectory
set(LIBSMACKER_SOURCES
    ../external/libsmack/smacker.c
    ../external/libsmack/smk_bitstream.c
    ../external/libsmack/smk_hufftree.c
)
set(LIBAUDIO_SOURCES
    ../external/audio/fileutil.c
    ../external/audio/wave.c
)

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
)

set(MACHINES_LIBS ${LIBS} PARENT_SCOPE)


# Includes
set(LOCAL_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/libdev
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/projects
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/libsmack
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
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
)


include_directories(${LOCAL_INCLUDES})
include_directories(SYSTEM ${SYSTEM_INCLUDES})


# Link directories
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/.
)


# Targets

add_library(base        OBJECT ${BASE_SOURCES})
add_library(mathex      OBJECT ${MATHEX_SOURCES})
add_library(afx         OBJECT ${AFX_SOURCES})
add_library(utility     OBJECT ${UTILITY_SOURCES})
add_library(profiler    OBJECT ${PROFILER_SOURCES})
add_library(system      OBJECT ${SYSTEM_SOURCES})
add_library(device      OBJECT ${DEVICE_SOURCES})
add_library(world4d     OBJECT ${WORLD4D_SOURCES})
add_library(gxin        OBJECT ${GXIN_SOURCES})
add_library(xin         OBJECT ${XIN_SOURCES})
add_library(stdlib      OBJECT ${STDLIB_SOURCES})
add_library(render      OBJECT ${RENDER_SOURCES})
add_library(sound       OBJECT ${SOUND_SOURCES})
add_library(network     OBJECT ${NETWORK_SOURCES})
add_library(ctl         OBJECT ${CTL_SOURCES})
add_library(phys        OBJECT ${PHYS_SOURCES})
add_library(sim         OBJECT ${SIM_SOURCES})
add_library(machgui     OBJECT ${MACHGUI_SOURCES})
add_library(gui         OBJECT ${GUI_SOURCES})
add_library(machphys    OBJECT ${MACHPHYS_SOURCES})
add_library(ani         OBJECT ${ANI_SOURCES})
add_library(recorder    OBJECT ${RECORDER_SOURCES})
add_library(envirnmt    OBJECT ${ENVIRNMT_SOURCES})
add_library(machlog     OBJECT ${MACHLOG_SOURCES})
add_library(libsmacker  OBJECT ${LIBSMACKER_SOURCES})
add_library(libaudio    OBJECT ${LIBAUDIO_SOURCES})

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



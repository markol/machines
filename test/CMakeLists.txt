include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/libdev
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libdev/device
)

enable_testing(true)

macro(append_test test_name test_srcs)
    set(TESTS_SRCS ${TESTS_SRCS} ${test_srcs})
    set(TESTS_NAME ${TESTS_NAME} ${test_name})
endmacro(append_test)

# Can't use `libs` because some libraries had to be substituted, like `afx` ':^/
# # You can still unit test afx lib's classes, except for AfxSdlApp
set(TESTING_THESE_GUYS
    base
    mathex
    afx_lite
    utility
    profiler
    system
    device
    world4d
    gxin
    xin
    stdlib
    render
    sound
    network
    ctl
    phys
    sim
    gui
    recorder
    envirnmt
    machphys
    machlog
    libs_mach_gui
)

#### Test Cases
#~:afx
append_test(AfxConfigurationTests afx/AfxConfigurationTests.cpp)
#~:ani
append_test(FrameScalerTests ani/FrameScalerTests.cpp)
#~:device
append_test(DevButtonEventTests device/DevButtonEventTests.cpp)
append_test(DevEventQueueTests device/DevEventQueueTests.cpp)
append_test(DevMouseTests device/DevMouseTests.cpp)
#~:system
append_test(SysPathNameTests system/SysPathNameTests.cpp)

#### Create executables
list(LENGTH TESTS_SRCS len1)
math(EXPR len2 "${len1} - 1")
foreach( val RANGE ${len2} )
    list(GET TESTS_SRCS ${val} test_src)
    list(GET TESTS_NAME ${val} test_name)
    add_executable(${test_name} AssertionOverrides.cpp ${test_src})
    target_link_libraries( ${test_name}
        gtest
        gmock
        gtest_main
        ${TESTING_THESE_GUYS}
        libaudio
        ${MACHINES_LIBS}
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

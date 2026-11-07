cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Threads REQUIRED)


find_package(SDL2 REQUIRED)
# Pi headers/libraries come from the toolchain file (CMAKE_SYSROOT) when
# cross-compiling, so no hard-coded rootfs path here.
include_directories(${SDL2_INCLUDE_DIRS})

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    message(STATUS "Building for x86_64 architecture")
    set(PLATFORM "x86")
else()
    set(PLATFORM arm)
    message(STATUS "Building for ARM architecture")
endif()

add_executable(pi-FMC AbstractFMC.h FMCManager.h FMCManager.cpp platform/KeypadScanner.h platform/${PLATFORM}/KeypadScanner.h platform/${PLATFORM}/KeypadScanner.cpp 
                LEDs.h LEDs.cpp MainFMC.h MainFMC.cpp  PiXPlaneFMCCDU.cpp Screen.h Screen.cpp 
                SplashFMC.h SplashFMC.cpp X737FMC.h X737FMC.cpp XfmcFMC.h XfmcFMC.cpp ZiboFMC.h ZiboFMC.cpp RotaryEncScanner.cpp RotaryEncScanner.h)


target_link_libraries(pi-FMC 
    Threads::Threads
    ${SDL2_LIBRARIES}
    SDL2_ttf
    freetype 
    z 
    png 
    XPlaneUDPClient 
    XPlaneExtPlaneClient
    )

if(PLATFORM STREQUAL "x86")
    # Real wiringPi only runs on a Pi; use the no-op stand-in in platform/x86.
    target_include_directories(pi-FMC BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform/x86)
else()
    target_link_libraries(pi-FMC wiringPi)
endif()

# Built under the name the Pi's service runs.
set_target_properties(pi-FMC PROPERTIES OUTPUT_NAME piXPlaneFMCCDU)

# Version: GitVersion.h with the current git commit, regenerated at every
# build (only rewritten when it changes).
set(GITVERSION_DIR ${CMAKE_BINARY_DIR}/generated)
add_custom_target(gitversion
    COMMAND ${CMAKE_COMMAND} -DSRC_DIR=${PROJECT_SOURCE_DIR}
            -DOUT_FILE=${GITVERSION_DIR}/GitVersion.h
            -P ${PROJECT_SOURCE_DIR}/cmake/GitVersion.cmake
    BYPRODUCTS ${GITVERSION_DIR}/GitVersion.h
    COMMENT "Checking git version")
add_dependencies(pi-FMC gitversion)
target_include_directories(pi-FMC PRIVATE ${GITVERSION_DIR})

# Hardware test programs (LEDs and keypad)
foreach(_test testFMCLED testFMCKeypad)
    add_executable(${_test} ${PROJECT_SOURCE_DIR}/src/${_test}/${_test}.cpp)
    if(PLATFORM STREQUAL "x86")
        target_include_directories(${_test} BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform/x86)
    else()
        target_link_libraries(${_test} wiringPi)
    endif()
endforeach()

# 'make install' (on the Pi) / deploy.sh (from a cross-build machine)
install(TARGETS pi-FMC testFMCLED testFMCKeypad RUNTIME DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/root/etc/systemd/system/piXPlaneFMCCDU.service
        DESTINATION /etc/systemd/system)
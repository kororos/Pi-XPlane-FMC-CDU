cmake_minimum_required(VERSION 3.10)
project(pi-FMC-CDU)
message("Starting Build Process...")
set(CMAKE_CXX_STANDARD 14)

# Optimised build by default (the original Makefile used -O3).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_subdirectory("./src/piXPlaneFMCCDU")


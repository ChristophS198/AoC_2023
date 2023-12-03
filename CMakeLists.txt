cmake_minimum_required(VERSION 3.0.0)
project(AoC_2020 VERSION 0.1.0 LANGUAGES C CXX)
set(EXECUTABLE_NAME "AoC_2020_main")

include(CTest)
enable_testing()

add_executable(${EXECUTABLE_NAME} main.cpp)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE DIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}")


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


# set c++ standard to c++-17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)


# Enable warnings
option(ENABLE_WARNINGS "Enable to add warnings to a target." ON)
option(ENABLE_WARNINGS_AS_ERRORS "Enable to treat warnings as errors." OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

if(${ENABLE_WARNINGS})
    include(helper) # include herlper.cmake 
    target_set_warnings(
        TARGET
        ${EXECUTABLE_NAME}
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

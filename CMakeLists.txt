CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(ini-cpp-parser)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

SET(DEFAULT_BUILD_TYPE "Release")
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        MESSAGE("Debug mode, enable gcov")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(LINK_LIBS "gcov")
    ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF()

add_executable(example example.cpp)

add_subdirectory(googletest)
add_subdirectory(test)
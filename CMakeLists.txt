cmake_minimum_required(VERSION 3.17)

project(libstormbird)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-wd"4068")
    add_definitions(-wd"4996")
    add_definitions(-wd"4251")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_definitions(-EHs)
    endif()
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions(-D_WIN32_CLANG)
    endif()
else(!USE_GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

if(CMAKE_SIZEOF_VOID_P LESS 8)
    message(FATAL_ERROR "Not a 64-bit compiler, pointer width is ${CMAKE_SIZEOF_VOID_P} bytes, expected at least 8.")
endif()

include(GenerateExportHeader)
include_directories(vendor)

add_subdirectory("vendor")
add_subdirectory("libstormbird")
add_subdirectory("cstormbird")

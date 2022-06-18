cmake_minimum_required(VERSION 3.6)
project(librtlgen DESCRIPTION "librtlgen static library")

set(EXECUTABLE_OUTPUT_PATH "../bin")
set(LIBRARY_OUTPUT_PATH "../bin")
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if(UNIX)
else()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wininclude
)


file(GLOB_RECURSE librtlgen_src
    LIST_DIRECTORIES false
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

if(UNIX)
else()
    foreach(_source IN ITEMS ${librtlgen_src})
        get_filename_component(_source_path "${_source}" PATH)
        # form proper filter name from the path to source file:
        string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        string(REPLACE "librtlgen" "" _group_path "${_group_path}")
        string(REPLACE "..\\" "" _group_path "${_group_path}")
 
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endif()


add_library(librtlgen STATIC
    ${librtlgen_src}
)


if(UNIX)
else()
    # MTd / MT flag in msvc
    set_target_properties(librtlgen PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


 
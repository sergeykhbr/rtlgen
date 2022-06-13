cmake_minimum_required(VERSION 3.6)
project(libscv_gen DESCRIPTION "libscv_gen shared library")

if (NOT DEFINED ENV{SYSTEMC_SRC} OR NOT DEFINED ENV{SYSTEMC_LIB})
    message(FATAL_ERROR "Variables SYSTEMC_SRC and SYSTEMC_LIB not defined.")
else()
    add_library(systemc STATIC IMPORTED)
    set_target_properties(systemc PROPERTIES IMPORTED_LOCATION $ENV{SYSTEMC_LIB}/Debug/systemc.lib)
endif()

set(src_top "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(sysc_lib "$ENV{SYSTEMC_LIB}")
set(EXECUTABLE_OUTPUT_PATH "../bin")
set(LIBRARY_OUTPUT_PATH "../bin")
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)

if(UNIX)
else()
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()


include_directories(
    $ENV{SYSTEMC_SRC}
    ${src_top}/common
)


file(GLOB_RECURSE libscv_gen_src
    LIST_DIRECTORIES false
    ${src_top}/common/*.cpp
    ${src_top}/common/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/exportmap.def
)

if(UNIX)
else()
    foreach(_source IN ITEMS ${libscv_gen_src})
        get_filename_component(_source_path "${_source}" PATH)
        # form proper filter name from the path to source file:
        string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        string(REPLACE "libscv_gen" "" _group_path "${_group_path}")
        string(REPLACE "..\\" "" _group_path "${_group_path}")
 
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endif()


if (MSVC)
    add_compile_options(/vmg /wd"4244" /wd"4996")
else()
    add_compile_options(-Wl,-Bstatic -lsystemc -Wl,-Bdynamic)
endif()


add_library(libscv_gen STATIC
    ${libscv_gen_src}
)

target_link_libraries(libscv_gen systemc)


if(UNIX)
else()
    # MTd / MT flag in msvc
    set_target_properties(libscv_gen PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


 
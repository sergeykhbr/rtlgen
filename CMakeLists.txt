cmake_minimum_required(VERSION 3.12)
project(rivergen)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(LIBRARY_OUTPUT_PATH "bin")
if(UNIX)
    add_compile_options(-rdynamic)
    set(INSTALL_DIR "bin")
else()
    add_definitions(-D_UNICODE)
    add_definitions(-DUNICODE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_subdirectory(librtlgen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/librtlgen/src
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

file(GLOB_RECURSE _rivergen_src
    LIST_DIRECTORIES false
    ${CMAKE_CURRENT_SOURCE_DIR}/rivergen/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rivergen/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.h
)


add_executable(
   rivergen
   ${_rivergen_src}
)

foreach(_source IN ITEMS ${_rivergen_src})
    get_filename_component(_source_path "${_source}" PATH)
    # form proper filter name from the path to source file:
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    string(REPLACE "..\\" "" _group_path "${_group_path}")
 
    source_group("${_group_path}" FILES "${_source}")
endforeach()


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT rivergen)

add_dependencies(rivergen librtlgen)
target_link_libraries(rivergen librtlgen)

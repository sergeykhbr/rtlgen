cmake_minimum_required(VERSION 3.6)
project(testsysvc)

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

add_subdirectory(libscv_gen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/testapp
    ${CMAKE_CURRENT_SOURCE_DIR}/libscv_gen/src
)


file(GLOB _testapp_src
	${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testapp/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/testapp/*.h
	)


add_executable(
   testapp
   ${_testapp_src}
)

if(UNIX)
    target_link_libraries(testapp sysvc)
else()

    foreach(_source IN ITEMS ${_testapp_src})
        get_filename_component(_source_path "${_source}" PATH)
        # form proper filter name from the path to source file:
        string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        string(REPLACE "..\\" "" _group_path "${_group_path}")
 
        source_group("${_group_path}" FILES "${_source}")
    endforeach()


    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testapp)

    add_dependencies(testapp libscv_gen)
    target_link_libraries(testapp libscv_gen)
endif()
  

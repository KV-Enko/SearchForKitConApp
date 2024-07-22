cmake_minimum_required(VERSION 3.10)

project(SearchForKitConApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SearchForKitConApp src/SearchForKitConApp.cpp)

if(WIN32)
	target_compile_options(SearchForKitConApp PRIVATE /utf-8)
	set_target_properties(SearchForKitConApp PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()


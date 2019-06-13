cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_source_files_properties(res.rc PROPERTIES LANGUAGE RC)

file(GLOB AST_SOURCES *.h *.hpp *.c *.cpp)

add_executable (ArmaShaderTool ${AST_SOURCES})

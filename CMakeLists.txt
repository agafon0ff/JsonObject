cmake_minimum_required(VERSION 3.5)

project(JsonObject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

add_definitions(-DTEST_JSON_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test.json")

add_executable(JsonObject main.cpp jsonobject.h jsonobject.cpp)

install(TARGETS JsonObject
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

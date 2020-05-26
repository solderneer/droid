cmake_minimum_required(VERSION 3.0)
project(droid)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Git REQUIRED)
add_subdirectory(contrib)

set( SOURCES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/leg.cpp )


add_executable(droid ${SOURCES_FILES})
target_include_directories(droid PUBLIC include)
target_link_libraries(droid cinder Cinder-ImGui)


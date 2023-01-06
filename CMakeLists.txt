cmake_minimum_required(VERSION 3.20)
project(demoA C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-std=c99")

set(LIB_SRC
        src/phreadTest.c)


add_library(demoA_lib ${LIB_SRC})
add_executable(demoA main.c)
target_link_libraries(demoA demoA_lib)

find_package(Threads REQUIRED)
target_link_libraries(demoA Threads::Threads)
#
add_library(lib_json cJSON.c)
target_link_libraries(demoA lib_json)

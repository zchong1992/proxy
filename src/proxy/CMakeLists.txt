cmake_minimum_required(VERSION 2.8)
project(proxy)

add_compile_options(-std=c++11  -g -O0)
message("add_compile_options(-std=c++11  -g -O0)")

include_directories(../../3rdparty/basesdk)

file(GLOB SRC *.cpp)
file(GLOB INC *.h)
message("build proxy src:" ${SRC})
message("build proxy inc:" ${INC})

add_executable(proxy ${SRC} ${INC} )
target_link_libraries(proxy baseservice_static pthread)

install(TARGETS proxy DESTINATION  "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/")

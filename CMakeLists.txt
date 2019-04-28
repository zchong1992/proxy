cmake_minimum_required(VERSION 2.8)
project(proxy)

#set(SRC src/*.cpp)
#set(INC src/*.h)


add_subdirectory(basesdk)


include_directories(basesdk)

file(GLOB SRC src/*.cpp)
file(GLOB INC src/*.h)
message("build proxy src:" ${SRC})
message("build proxy inc:" ${INC})

add_executable(proxy ${SRC} ${INC} )
target_link_libraries(proxy baseservice pthread)

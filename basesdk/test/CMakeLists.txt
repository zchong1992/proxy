cmake_minimum_required(VERSION 2.8)
project(testLib)

INCLUDE_DIRECTORIES( ..)
set(src testmain.cpp)

add_definitions(-g -O0)
add_executable(testLib ${src})
target_link_libraries(testLib baseservice)

find_package(Threads REQUIRED)  
if(THREADS_HAVE_PTHREAD_ARG)  
  set_property(TARGET testLib PROPERTY COMPILE_OPTIONS "-pthread")  
  set_property(TARGET testLib PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")  
endif()  
if(CMAKE_THREAD_LIBS_INIT)  
  target_link_libraries(testLib "${CMAKE_THREAD_LIBS_INIT}")  
endif()  



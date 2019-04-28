cmake_minimum_required(VERSION 2.5)
project(baseservice)

if (UNIX)
add_definitions(-std=c++11)
add_definitions(-g -O0)
endif  (UNIX)

set(incPath . .. ../inc)
INCLUDE_DIRECTORIES( ${incPath})
set(SRC aes.cpp 
	base64.cpp
	baseDataType.cpp
	config.cpp
	dir.cpp
	global.cpp
	img.cpp
	libbaseservice.cpp
	log.cpp
	zlibnet.cpp
	cThread.cpp
	msgTransport.cpp
	segmentfault.cpp)
set(INC 
	../inc/aes.h
	../inc/cThread.h
	../inc/msgTransport.h
	../inc/base64.h
	../inc/baseDataType.h
	../inc/config.h
	../inc/dir.h
	../inc/global.h
	../inc/img.h
	../inc/log.h
	../inc/segmentfault.h
	../inc/pointer.h
	../inc/zlibnet.h
	../libbaseservice.h)
#set(outPutDir ${PROJECT_BINARY_DIR})
set(outPutDir ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "Project Directory: ${outPutDir}")


set(LIBS "")



if (WIN32)
set(LIBS "${LIBS} wsock32")
endif  (WIN32)


add_library(baseservice SHARED ${SRC} ${INC} )
add_library(baseservice_static STATIC ${SRC} ${INC} )
target_link_libraries(baseservice  ${LIBS})
target_link_libraries(baseservice_static  ${LIBS})

set_target_properties(baseservice_static PROPERTIES OUTPUT_NAME "baseservice")
set_target_properties(baseservice PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(baseservice_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(baseservice PROPERTIES VERSION 1.0 SOVERSION 1)

#ADD_LIBRARY(baseservice STATIC ${SRC} ${INC})  

install(TARGETS  baseservice_static	    ARCHIVE DESTINATION ${outPutDir}/../lib/)
#install(TARGETS  baseservice		    LIBRARY DESTINATION ${outPutDir}/../lib/)
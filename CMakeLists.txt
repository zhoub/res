cmake_minimum_required(VERSION 3.8)

project(res)

if(NOT DEFINED WITH_TESTS)
set(WITH_TESTS OFF)
endif()

set(RES_INCLUDE_FILES
	img/file_data.h
	img/img_data.h
)

set(RES_SOURCE_FILES
	img/impl/file_data.cpp
	img/impl/img_data.cpp
	img/impl/inner_include.h
	img/impl/bmp.cpp
	img/impl/png.cpp
	img/impl/tga.cpp
)

set(ZLIB_SOURCE_FILES
	zlib/adler32.c
	zlib/compress.c
	zlib/crc32.c
	zlib/crc32.h
	zlib/deflate.c
	zlib/deflate.h
	zlib/gzguts.h
	zlib/infback.c
	zlib/inffast.c
	zlib/inffast.h
	zlib/inffixed.h
	zlib/inflate.c
	zlib/inflate.h
	zlib/inftrees.c
	zlib/inftrees.h
	zlib/trees.c
	zlib/trees.h
	zlib/uncompr.c
	zlib/zconf.h
	zlib/zlib.h
	zlib/zutil.c
	zlib/zutil.h
)

source_group(img FILES ${RES_INCLUDE_FILES})
source_group(source\\img FILES ${RES_SOURCE_FILES})
source_group(zlib FILES ${ZLIB_SOURCE_FILES})
add_library(res ${ZLIB_SOURCE_FILES} ${RES_INCLUDE_FILES} ${RES_SOURCE_FILES})

if(WITH_TESTS)

message(STATUS "with-tests building test project")

if(MSVC)
set(RES_TEST_SOURCE_FILES
	win32_test/winmain.cpp
	win32_test/test.cpp	
)
include_directories(res_test .)
add_executable(res_test WIN32 ${RES_TEST_SOURCE_FILES})
set_target_properties(res_test PROPERTIES COMPILE_DEFINITIONS UNICODE)
set_target_properties(res_test PROPERTIES COMPILE_DEFINITIONS _UNICODE)
set_target_properties(res_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win32_test)
target_link_libraries(res_test res)

endif(MSVC)

endif(WITH_TESTS)
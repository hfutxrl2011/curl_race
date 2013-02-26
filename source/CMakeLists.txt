
cmake_minimum_required(VERSION 2.8)

project(curl_race)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

if (MSVC)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/bin/cmake/win/modules/")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -DCURL_STATICLIB ")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(CMAKE_PREFIX_PATH
		${CMAKE_SOURCE_DIR}/lib/curl-7.29.0/win
		${CMAKE_PREFIX_PATH}
	)
endif()

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(curl_race main_curl_race.cpp)

target_link_libraries(curl_race
	${CURL_LIBRARY}
)


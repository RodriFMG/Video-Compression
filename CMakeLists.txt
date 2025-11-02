cmake_minimum_required(VERSION 3.26)
project(Video_Compression)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 14)

add_executable(Video_Compression
        main.cpp
        Encoding.h
        Encoding.cpp
        GOP.h
        GOP.cpp)

target_link_libraries( Video_Compression ${OpenCV_LIBS} )

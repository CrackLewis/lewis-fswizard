cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_MAKEFILE_CODING "gbk")

project(fswizard VERSION 0.1.0)

aux_source_directory(src/common SRC_COMMON)
aux_source_directory(src/fs/v6pp SRC_FS_V6PP)
aux_source_directory(src/fs/vhdx SRC_FS_VHDX)
aux_source_directory(src/io SRC_IO)
aux_source_directory(src/util SRC_UTIL)

add_executable(fswizard src/main.cpp ${SRC_COMMON} ${SRC_FS_V6PP} ${SRC_IO} ${SRC_UTIL})
target_include_directories(fswizard PRIVATE include)

add_executable(testargs src/app/testargs.cpp src/common/argparse.cpp)
target_include_directories(testargs PRIVATE include)
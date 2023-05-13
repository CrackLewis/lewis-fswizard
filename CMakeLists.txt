cmake_minimum_required(VERSION 3.16.0)

# set(CMAKE_MAKEFILE_CODING "gbk")

add_compile_options(-std=c++17)

project(fswizard VERSION 0.1.0)

aux_source_directory(src/common SRC_COMMON)
aux_source_directory(src/fs/v6pp SRC_FS_V6PP)
aux_source_directory(src/fs/vhdx SRC_FS_VHDX)
aux_source_directory(src/io SRC_IO)
aux_source_directory(src/util SRC_UTIL)

set(SRC_V6PP ${SRC_COMMON} ${SRC_FS_V6PP} ${SRC_IO} ${SRC_UTIL})

# add_executable(testargs src/app/testargs.cpp src/common/argparse.cpp)
# target_include_directories(testargs PRIVATE include)

# add_executable(testimage src/app/testimage.cpp ${SRC_V6PP})
# target_include_directories(testimage PRIVATE include)

# add_executable(testvfs src/app/testvfs.cpp ${SRC_V6PP})
# target_include_directories(testvfs PRIVATE include)

# add_executable(alterimage src/app/alterimage.cpp ${SRC_V6PP})
# target_include_directories(alterimage PRIVATE include)

add_executable(v6pp-fs-local src/app/v6pp-fs-local.cpp ${SRC_V6PP})
target_include_directories(v6pp-fs-local PRIVATE include)

add_executable(makeimage src/app/makeimage.cpp ${SRC_V6PP})
target_include_directories(makeimage PRIVATE include)

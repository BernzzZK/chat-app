#cmake_minimum_required(VERSION 3.15)
#project(ChatSystem)
#
## 全局设置
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## 设置输出目录
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/HeadFile)
#
## 添加子目录
#add_subdirectory(LibSrc)
#add_subdirectory(server)
#add_subdirectory(client)

cmake_minimum_required(VERSION 3.15)
project(ChatSystem VERSION 1.0.0 LANGUAGES CXX)

# 全局设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找依赖
find_package(Threads REQUIRED)

# 自定义查找 hiredis 和 mysqlclient
find_library(HIREDIS_LIB hiredis)
find_library(MYSQLCLIENT_LIB mysqlclient)

if(NOT HIREDIS_LIB OR NOT MYSQLCLIENT_LIB)
    message(FATAL_ERROR "Required libraries not found: hiredis or mysqlclient")
endif()

# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/HeadFile)

# 添加子目录 - 顺序很重要
add_subdirectory(LibSrc)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(test)

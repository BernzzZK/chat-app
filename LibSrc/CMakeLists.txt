cmake_minimum_required(VERSION 3.15)
project(request)

aux_source_directory(./ SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})
#project(ChatClient)
## 显式指定源文件
#aux_source_directory(./ SRC)
#aux_source_directory(../LibSrc LIBSRC)
#
## 创建可执行文件
#add_executable(${PROJECT_NAME} ${SRC} ${LIBSRC})
#
## 头文件包含
#include_directories(
#        ${CMAKE_CURRENT_SOURCE_DIR}/HeadFile
#        ../HeadFile
#)
#
#target_link_libraries(${PROJECT_NAME} PRIVATE
#        muduo_net
#        muduo_base
#        pthread
#        mysqlclient
#        hiredis
#)

# 客户端项目配置
project(ChatClient LANGUAGES CXX)

# 收集源文件
aux_source_directory(. CLIENT_SOURCES)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../HeadFile
)

# 链接依赖
target_link_libraries(${PROJECT_NAME} PRIVATE
        ChatLib
        muduo_net
        muduo_base
        ${HIREDIS_LIB}
        ${MYSQLCLIENT_LIB}
        Threads::Threads
)


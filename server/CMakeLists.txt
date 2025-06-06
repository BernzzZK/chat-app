#project(ChatServer)
## 显式指定源文件
#aux_source_directory(./ SRC)
#aux_source_directory(../LibSrc LIBSRC)
#
## 创建可执行文件
#add_executable(${PROJECT_NAME} ${SRC} ${LIBSRC})
#
## 头文件包含
#include_directories(
#    ${CMAKE_CURRENT_SOURCE_DIR}/HeadFile
#    ../HeadFile
#)
#
#target_link_libraries(${PROJECT_NAME} PRIVATE
#    muduo_net
#    muduo_base
#    pthread
#    mysqlclient
#    hiredis
#)

# 服务器项目配置
project(ChatServer LANGUAGES CXX)

# 收集源文件
aux_source_directory(. SERVER_SOURCES)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SERVER_SOURCES})

# 链接依赖
target_link_libraries(${PROJECT_NAME} PRIVATE
        ChatLib
        muduo_net
        muduo_base
        yaml-cpp
        ${HIREDIS_LIB}
        ${MYSQLCLIENT_LIB}
        Threads::Threads
)

# 安装规则（可选）
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
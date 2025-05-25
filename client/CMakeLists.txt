project(ChatClient)

# 显式指定源文件
aux_source_directory(./ SRC)
aux_source_directory(../LibSrc LIBSRC)

# 源文件列表
set(SRC_FILES
        mainclient.cpp
        Client.cpp
        ${SRC}
        ${LIBSRC}
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 包含目录
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/HeadFile
        ../HeadFile
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
        muduo_net
        muduo_base
        pthread
        hiredis
        mysqlclient
)
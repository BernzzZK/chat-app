project(ChatClient)
# 显式指定源文件
aux_source_directory(./ SRC)
aux_source_directory(../LibSrc LIBSRC)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC} ${LIBSRC})

# 头文件包含
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/HeadFile
        ../HeadFile
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        muduo_net
        muduo_base
        pthread
        mysqlclient
        hiredis
)

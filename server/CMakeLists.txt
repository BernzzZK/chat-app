# 服务器子项目
project(ChatServer)

# 显式指定源文件
aux_source_directory(./ SRC)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC})

# 头文件包含
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    muduo_net
    muduo_base
    pthread
)
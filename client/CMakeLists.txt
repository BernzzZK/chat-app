project(ChatClient)

# 源文件列表
set(SRC_FILES
    mainclient.cpp
    Client.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    muduo_net
    muduo_base
    pthread
)
cmake_minimum_required(VERSION 3.15)
project(ChatLib)

aux_source_directory(./ SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE
        hiredis       # 提供 redisCommand 等函数
        mysqlclient   # 提供 mysql_real_connect 等函数
        pthread       # 可能需要的线程库
)
#cmake_minimum_required(VERSION 3.15)
#project(ChatLib)
#
#aux_source_directory(./ SRC)
#add_library(${PROJECT_NAME} SHARED ${SRC})
#
#target_link_libraries(${PROJECT_NAME} PRIVATE
#        hiredis       # 提供 redisCommand 等函数
#        mysqlclient   # 提供 mysql_real_connect 等函数
#        pthread       # 可能需要的线程库
#)

# 库项目配置
project(ChatLib LANGUAGES CXX)

# 明确列出源文件（比GLOB更可靠）
set(LIB_SOURCES
        base/Common.cpp
        base/Login.cpp
        base/LoginReq.cpp
        db/MysqlConn.cpp
        db/MysqlConnPool.cpp
        db/RedisConn.cpp
        base/RegisterReq.cpp
        base/Request.cpp
        base/Response.cpp
        client/UserFunction.cpp
        base/Register.cpp
        base/LogoutReq.cpp
        base/Logout.cpp
        server/UniqueHashGenerator.cpp
        base/AddFriendReq.cpp
        base/AddFriend.cpp
        db/RedisConnPool.cpp
        base/SendMsgReq.cpp
        base/Message.cpp
        base/FriendApplication.cpp
        base/FriendList.cpp
        base/ChangeUserNameReq.cpp
        base/ChangeUserName.cpp
        base/HeartBeat.cpp
        ../HeadFile/Req.h
)

# 创建共享库
add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})

# 仅设置必要的属性（简化版）
if(PROJECT_VERSION)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            VERSION "${PROJECT_VERSION}"
    )
endif()

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../HeadFile
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${HIREDIS_LIB}
        ${MYSQLCLIENT_LIB}
        Threads::Threads
)
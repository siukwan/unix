PROJECT (UNIX)
#显示路径信息
MESSAGE(STATUS "UNIX:This is BINARY dir" ${UNIX_BINARY_DIR})
MESSAGE(STATUS "UNIX:This is SOURCE dir" ${UNIX_SOURCE_DIR})

#设置客户端编译文件
SET(CLIENT_SRC_LIST tcpcli.cpp  func_err.cpp func_wrap.cpp)

#设置服务器编译文件
SET(SERVER_SRC_LIST tcpserv.cpp func_err.cpp func_wrap.cpp)

#输出可执行文件
ADD_EXECUTABLE(client ${CLIENT_SRC_LIST})
ADD_EXECUTABLE(server ${SERVER_SRC_LIST})








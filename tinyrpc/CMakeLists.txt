add_library(tinyrpc STATIC
        util.h  server/RpcService.h
        server/BaseServer.cc server/BaseServer.h
        server/RpcServer.cc server/RpcServer.h
        server/Procedure.h client/BaseClient.cc client/BaseClient.h)
target_link_libraries(tinyrpc tinynet_net tinynet_base)

add_subdirectory(stubgenerator)
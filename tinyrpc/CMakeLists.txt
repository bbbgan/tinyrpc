add_library(tinyrpc STATIC
        util.h  server/RpcService.h
        server/BaseServer.cc server/BaseServer.h
        server/RpcServer.cc server/RpcServer.h
        server/Procedure.h client/BaseClient.cc client/BaseClient.h)
target_link_libraries(tinyrpc tinynet_net tinynet_base)
# install(TARGETS tinyrpc DESTINATION lib)

# set(HEADERS
#         util.h
#         server/RpcServer.h
#         server/BaseServer.h
#         server/Procedure.h
#         server/RpcService.h
#         client/BaseClient.h)
# install(FILES ${HEADERS} DESTINATION include)

add_subdirectory(stubgenerator)
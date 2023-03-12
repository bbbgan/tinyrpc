add_executable(tinyrpcstub
        StubGenerator.h
        StubGenerator.cc
        ServiceStubGenerator.h
        ServiceStubGenerator.cc
        ClientStubGenerator.h
        ClientStubGenerator.cc
        main.cc
        )

target_link_libraries(tinyrpcstub )
# install(TARGETS tinyrpcstub DESTINATION bin)
add_custom_command(
        OUTPUT Header
        COMMAND tinyrpcstub -c -s -i ${CMAKE_CURRENT_SOURCE_DIR}/test.json
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/test.json
        DEPENDS tinyrpcstub
        COMMENT "Generating Server/Client Stub..."
        VERBATIM
)

add_custom_command(
        OUTPUT TESTClientStub.h TESTServiceStub.h
        COMMAND ${CMAKE_CODE_FORMATTER}
        ARGS -i TESTClientStub.h TESTServiceStub.h
        DEPENDS Header
        COMMENT "Formatting Server/Client Stub..."
        VERBATIM
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_executable(test_tinyrpc
        test_tinyrpc.cc
        main.cc
        )
target_link_libraries(test_tinyrpc tinyrpc tinynet_net tinynet_base)


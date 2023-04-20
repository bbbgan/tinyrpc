add_custom_command(
        OUTPUT Header
				COMMAND tinyrpcstub -c -s -i ${CMAKE_CURRENT_SOURCE_DIR}/spec.json
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/spec.json
				DEPENDS tinyrpcstub
        COMMENT "Generating Server/Client Stub..."
        VERBATIM
)

add_custom_command(
				OUTPUT ArithmeticServiceStub.h ArithmeticClientStub.h
        COMMAND ${CMAKE_CODE_FORMATTER}
        ARGS -i ArithmeticServiceStub.h ArithmeticClientStub.h
        DEPENDS Header
        COMMENT "Formatting Server/Client Stub..."
        VERBATIM
)

add_executable(arithmetic_server ArithmeticService.cc ArithmeticServiceStub.h)
target_link_libraries(arithmetic_server tinyrpc tinynet_net tinynet_base)

add_executable(arithmetic_client ArithmeticClient.cc ArithmeticClientStub.h)
target_link_libraries(arithmetic_client tinyrpc tinynet_net tinynet_base)
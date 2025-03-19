add_executable(lexer_example
    lexer_example.cpp
)

add_executable(number_literal_example
    number_literal_example.cpp
)

add_executable(string_literal_example
    string_literal_example.cpp
)

add_executable(identifier_keyword_example
    identifier_keyword_example.cpp
)

target_link_libraries(lexer_example
    PRIVATE
    frontend
)

target_link_libraries(number_literal_example
    PRIVATE
    frontend
)

target_link_libraries(string_literal_example
    PRIVATE
    frontend
)

target_link_libraries(identifier_keyword_example
    PRIVATE
    frontend
)

target_include_directories(number_literal_example
    PRIVATE
    ${RP_INCLUDE_DIR}
)

target_include_directories(string_literal_example
    PRIVATE
    ${RP_INCLUDE_DIR}
)

target_include_directories(identifier_keyword_example
    PRIVATE
    ${RP_INCLUDE_DIR}
)

target_include_directories(lexer_example
    PRIVATE
    ${RP_INCLUDE_DIR}
)



set_target_properties(
    lexer_example
    number_literal_example
    string_literal_example
    identifier_keyword_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/example/bin"
)

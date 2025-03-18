# target_sources(frontend
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/ExprParser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/InitializerParser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/StmtParser.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/TypeParser.cpp
# )

# target_include_directories(frontend
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
add_library(Parser INTERFACE)
# add_subdirectory(AST)
# add_subdirectory(ClassDecl)
add_subdirectory(Lexer)
# add_subdirectory(VarDecl)

target_link_libraries(Parser INTERFACE Lexer)

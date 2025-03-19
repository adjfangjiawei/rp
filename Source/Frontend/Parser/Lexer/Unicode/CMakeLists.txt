
add_library(LexerUnicode
    Core/UnicodeCore.cpp
    Categories/UnicodeCategories.cpp
    Processing/UnicodeProcessing.cpp
    Scanner/UTF8Scanner.cpp
    Escape/UnicodeEscape.cpp
)

target_include_directories(LexerUnicode
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
         ${RP_INCLUDE_DIR}
)
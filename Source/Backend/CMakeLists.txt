add_library(backend STATIC
    IR/IR.cpp
    CodeGen/CodeGen.cpp
)

target_include_directories(backend
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(IR)
add_subdirectory(CodeGen)

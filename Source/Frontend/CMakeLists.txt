

# Create the frontend library
add_library(frontend INTERFACE)
add_subdirectory(Diagnostic)
# Add subdirectories
add_subdirectory(Parser)
# add_subdirectory(Sema)


# Set include directories for frontend library
target_include_directories(frontend
    INTERFACE
    ${RP_INCLUDE_DIR}
)

# Link all frontend components
target_link_libraries(frontend INTERFACE
    INTERFACE
    Parser
)


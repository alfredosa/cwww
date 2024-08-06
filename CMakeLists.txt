cmake_minimum_required(VERSION 3.25)
project(c-http-bloggaru)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all .c files in the src directory and subdirectories, excluding main.c
file(GLOB_RECURSE SOURCE_FILES 
    "src/*.c"
)
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

# Remove any CMake-generated files
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*CMakeFiles.*")

# Main executable
add_executable(${PROJECT_NAME}
    src/main.c
    ${SOURCE_FILES}
)

# Copy templates and static directories to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/templates DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/static DESTINATION ${CMAKE_BINARY_DIR})

#### TESTING SECTION :)

# # Enable testing
# enable_testing()
#
# # Test executable
# add_executable(RunTests
#     ${SOURCE_FILES}
# )
# target_compile_definitions(RunTests PRIVATE TESTING)


cmake_minimum_required(VERSION 3.25)
project(c-http-bloggaru)

# Allow user to specify build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set compile options based on the build type
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find md4c
find_package(PkgConfig REQUIRED)
pkg_check_modules(MD4C REQUIRED md4c md4c-html)

# Find SQLite3
find_package(SQLite3 REQUIRED)

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

# Include directories for md4c and SQLite3
target_include_directories(${PROJECT_NAME} PRIVATE ${MD4C_INCLUDE_DIRS} ${SQLite3_INCLUDE_DIRS})

# Link against md4c and SQLite3
target_link_directories(${PROJECT_NAME} PRIVATE ${MD4C_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${MD4C_LIBRARIES} SQLite::SQLite3)

# Explicitly link against md4c-html if it's not included in MD4C_LIBRARIES
if(NOT ";${MD4C_LIBRARIES};" MATCHES ";md4c-html;")
    target_link_libraries(${PROJECT_NAME} PRIVATE md4c-html)
endif()

# Copy templates and static directories to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/templates DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/static DESTINATION ${CMAKE_BINARY_DIR})

# Add custom target for running with Valgrind (Debug mode only)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(run_valgrind
        COMMAND valgrind --leak-check=full --show-leak-kinds=all $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
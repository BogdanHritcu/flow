cmake_minimum_required(VERSION 3.26)

project(sandbox LANGUAGES C CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

add_executable(sandbox)

set(SANDBOX_SOURCES "src/entry.cpp"
        "src/tests/physics_test.hpp")

set(SANDBOX_HEADERS)

target_sources(sandbox PRIVATE ${SANDBOX_SOURCES})
target_sources(sandbox PRIVATE FILE_SET HEADERS FILES ${SANDBOX_HEADERS})

target_link_libraries(sandbox PRIVATE flow)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SANDBOX_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SANDBOX_HEADERS})

set_target_properties(sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:sandbox>"
        WIN32_EXECUTABLE "$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release>:TRUE>>")

set(SANDBOX_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(SANDBOX_TARGET_DATA_DIR "$<TARGET_FILE_DIR:sandbox>/data")

add_custom_command(TARGET sandbox POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
        -E copy_directory_if_different "${FLOW_DATA_DIR}" "${SANDBOX_TARGET_DATA_DIR}/flow"
        COMMAND "${CMAKE_COMMAND}"
        -E copy_directory_if_different "${SANDBOX_DATA_DIR}" "${SANDBOX_TARGET_DATA_DIR}"
        COMMENT "Copying missing data from ${FLOW_DATA_DIR} and ${SANDBOX_DATA_DIR} to ${SANDBOX_TARGET_DATA_DIR}"
        VERBATIM)

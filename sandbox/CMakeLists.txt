cmake_minimum_required(VERSION 3.26)

project(sandbox LANGUAGES C CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_executable(sandbox)

set(SANDBOX_SOURCES "src/entry.cpp")

set(SANDBOX_HEADERS "src/chunk.hpp"
                    "src/packed_chunk.hpp"
                    "src/tile.hpp"
                    "src/tilemap.hpp"
                    "src/tests/batch_renderer/textured_quads_test.hpp"
                    "src/tests/partitioner/allocation_test.hpp")

target_sources(sandbox PRIVATE ${SANDBOX_SOURCES})
target_sources(sandbox PRIVATE FILE_SET HEADERS FILES ${SANDBOX_HEADERS})

target_link_libraries(sandbox PRIVATE flow)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SANDBOX_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SANDBOX_HEADERS})

add_custom_command(TARGET sandbox POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                   "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/data")

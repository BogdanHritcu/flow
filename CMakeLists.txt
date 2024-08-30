cmake_minimum_required(VERSION 3.25)

project(flow LANGUAGES C CXX)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FLOW_BUILD_SHARED "Build shared lib" OFF)
option(FLOW_BUILD_SANDBOX "Build sandbox" OFF)

if (FLOW_BUILD_SHARED OR BUILD_SHARED_LIBS)
    add_library(flow SHARED)
else ()
    add_library(flow STATIC)
endif ()

add_library(flow::flow ALIAS flow)

set(FLOW_SOURCES "src/core/application.cpp"
        "src/core/entry.cpp"
        "src/core/window.cpp"
        "src/utility/uuid.cpp")

set(FLOW_HEADERS "include/flow/core/application.hpp"
        "include/flow/core/assertion.hpp"
        "include/flow/core/defines.hpp"
        "include/flow/core/engine_interface.hpp"
        "include/flow/core/input_system_interface.hpp"
        "include/flow/core/input_system.hpp"
        "include/flow/core/logger.hpp"
        "include/flow/core/window_interface.hpp"
        "include/flow/core/window.hpp"
        "include/flow/graphics/opengl/buffer.hpp"
        "include/flow/graphics/opengl/commands.hpp"
        "include/flow/graphics/opengl/enum_types.hpp"
        "include/flow/graphics/opengl/fence.hpp"
        "include/flow/graphics/opengl/shader.hpp"
        "include/flow/graphics/opengl/texture.hpp"
        "include/flow/graphics/opengl/vertex_array.hpp"
        "include/flow/graphics/opengl/vertex_attribute.hpp"
        "include/flow/graphics/renderer/instance_renderer.hpp"
        "include/flow/graphics/renderer/line_renderer.hpp"
        "include/flow/graphics/renderer/rectangle_renderer.hpp"
        "include/flow/graphics/renderer/renderer_config.hpp"
        "include/flow/graphics/renderer/sprite_renderer.hpp"
        "include/flow/graphics/sprite/sprite_animation.hpp"
        "include/flow/graphics/texture/texture_atlas.hpp"
        "include/flow/graphics/texture/image.hpp"
        "include/flow/graphics/orthographic_camera.hpp"
        "include/flow/input/binding_context.hpp"
        "include/flow/input/binding_enums.hpp"
        "include/flow/input/binding.hpp"
        "include/flow/input/input_context.hpp"
        "include/flow/math/vec2.hpp"
        "include/flow/math/vec2_math.hpp"
        "include/flow/utility/animation.hpp"
        "include/flow/utility/animation_controller.hpp"
        "include/flow/utility/animation_queue.hpp"
        "include/flow/utility/bounded_cursor.hpp"
        "include/flow/utility/buddy_partitioner.hpp"
        "include/flow/utility/compressed_pair.hpp"
        "include/flow/utility/concepts.hpp"
        "include/flow/utility/curve.hpp"
        "include/flow/utility/dense_tree.hpp"
        "include/flow/utility/ease.hpp"
        "include/flow/utility/filesystem.hpp"
        "include/flow/utility/fixed_point.hpp"
        "include/flow/utility/helpers.hpp"
        "include/flow/utility/integer_range.hpp"
        "include/flow/utility/iostream_view.hpp"
        "include/flow/utility/istream_view.hpp"
        "include/flow/utility/noise.hpp"
        "include/flow/utility/numeric.hpp"
        "include/flow/utility/ostream_view.hpp"
        "include/flow/utility/pair_serialization.hpp"
        "include/flow/utility/path_serialization.hpp"
        "include/flow/utility/random.hpp"
        "include/flow/utility/record.hpp"
        "include/flow/utility/serialization.hpp"
        "include/flow/utility/sized_int.hpp"
        "include/flow/utility/sliding_stream_buffer.hpp"
        "include/flow/utility/sliding_window.hpp"
        "include/flow/utility/stopwatch.hpp"
        "include/flow/utility/stream_algorithm.hpp"
        "include/flow/utility/string_serialization.hpp"
        "include/flow/utility/time.hpp"
        "include/flow/utility/traits.hpp"
        "include/flow/utility/unique_handle.hpp"
        "include/flow/utility/unit.hpp"
        "include/flow/utility/unordered_map_serialization.hpp"
        "include/flow/utility/uuid.hpp"
        "include/flow/utility/vector_serialization.hpp")

set(FLOW_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
if (NOT PROJECT_IS_TOP_LEVEL)
    set(FLOW_DATA_DIR "${FLOW_DATA_DIR}" PARENT_SCOPE)
endif ()

target_sources(flow PRIVATE ${FLOW_SOURCES})
target_sources(flow PUBLIC FILE_SET HEADERS FILES ${FLOW_HEADERS})

target_include_directories(flow INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_definitions(flow PUBLIC "$<$<CONFIG:Debug,RelWithDebInfo>:FLOW_DEBUG>"
        "$<$<CONFIG:Release,RelWithDebInfo>:FLOW_RELEASE>")

target_compile_definitions(flow PUBLIC "FLOW_ENABLE_LOG_CRITICAL"
        "FLOW_ENABLE_LOG_ERROR"
        "FLOW_ENABLE_LOG_WARN"
        "FLOW_ENABLE_LOG_INFO"
        "FLOW_ENABLE_LOG_DEBUG"
        "FLOW_ENABLE_LOG_TRACE")

target_compile_definitions(flow PUBLIC "$<$<PLATFORM_ID:Android>:FLOW_PLATFORM_ANDROID>"
        "$<$<PLATFORM_ID:Darwin>:FLOW_PLATFORM_MACOS>"
        "$<$<PLATFORM_ID:iOS>:FLOW_PLATFORM_IOS>"
        "$<$<PLATFORM_ID:Linux>:FLOW_PLATFORM_LINUX>"
        "$<$<PLATFORM_ID:Windows>:FLOW_PLATFORM_WINDOWS>")

target_compile_options(flow PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${FLOW_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${FLOW_HEADERS})

# add external dependencies

# glad
add_subdirectory(extern/glad)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Install GLFW" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC" CACHE STRING "Build GLFW as static lib" FORCE)
add_subdirectory(extern/glfw)

# glm
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++20" FORCE)
set(GLM_ENABLE_FAST_MATH ON CACHE BOOL "Enable fast math" FORCE)
set(GLM_ENABLE_LANG_EXTENSIONS ON CACHE BOOL "Enable language extensions" FORCE)
set(GLM_ENABLE_SIMD_AVX ON CACHE BOOL "Enable AVX optimizations" FORCE)
set(GLM_ENABLE_SIMD_AVX2 ON CACHE BOOL "Enable AVX2 optimizations" FORCE)
set(GLM_ENABLE_SIMD_SSE2 ON CACHE BOOL "Enable SSE2 optimizations" FORCE)
set(GLM_ENABLE_SIMD_SSE3 ON CACHE BOOL "Enable SSE3 optimizations" FORCE)
set(GLM_ENABLE_SIMD_SSE4_1 ON CACHE BOOL "Enable SSE4.1 optimizations" FORCE)
set(GLM_ENABLE_SIMD_SSE4_2 ON CACHE BOOL "Enable SSE4.2 optimizations" FORCE)
set(GLM_ENABLE_SIMD_SSSE3 ON CACHE BOOL "Enable SSSE3 optimizations" FORCE)

add_subdirectory(extern/glm)
target_compile_definitions(flow PUBLIC "GLM_ENABLE_EXPERIMENTAL"
        "GLM_FORCE_ALIGNED_GENTYPES")

# stb
add_subdirectory(extern/stb)

# spdlog
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of fmt library" FORCE)

add_subdirectory(extern/spdlog)
target_compile_definitions(flow PUBLIC "B2_USER_SETTINGS")

target_link_libraries(flow PUBLIC glad
        glfw
        glm
        stb
        spdlog)

set_target_properties(glad glfw update_mappings glm stb spdlog PROPERTIES FOLDER "extern")

# sandbox

if (FLOW_BUILD_SANDBOX)
    add_subdirectory(sandbox)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)
endif ()

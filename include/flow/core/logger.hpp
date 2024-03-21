#pragma once

#include <spdlog/spdlog.h>

#if defined(FLOW_ENABLE_LOG_CRITICAL)
#  define FLOW_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#else
#  define FLOW_LOG_CRITICAL(...) (void)0
#endif

#if defined(FLOW_ENABLE_LOG_ERROR)
#  define FLOW_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#else
#  define FLOW_LOG_ERROR(...) (void)0
#endif

#if defined(FLOW_ENABLE_LOG_WARN)
#  define FLOW_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#else
#  define FLOW_LOG_WARN(...) (void)0
#endif

#if defined(FLOW_ENABLE_LOG_INFO)
#  define FLOW_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#else
#  define FLOW_LOG_INFO(...) (void)0
#endif

#if defined(FLOW_ENABLE_LOG_DEBUG)
#  define FLOW_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#else
#  define FLOW_LOG_DEBUG(...) (void)0
#endif

#if defined(FLOW_ENABLE_LOG_TRACE)
#  define FLOW_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#else
#  define FLOW_LOG_TRACE(...) (void)0
#endif

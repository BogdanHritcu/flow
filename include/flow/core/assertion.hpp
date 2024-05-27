#pragma once

#ifdef FLOW_DEBUG
#  include <format>

#  include "logger.hpp"

#  ifdef _MSC_VER
#    define FLOW_DEBUG_BREAK __debugbreak
#  else
#    define FLOW_DEBUG_BREAK __builtin_trap
#  endif

// NOLINTBEGIN
#  define FLOW_ASSERT(expression, message, ...)                                          \
      do                                                                                 \
      {                                                                                  \
          if (!(expression))                                                             \
          {                                                                              \
              FLOW_LOG_CRITICAL(                                                         \
                  "assertion failure: '{}' in '{}', file {} at line {}: '{}' was false", \
                  std::format(message __VA_OPT__(, ) __VA_ARGS__),                       \
                  __func__,                                                              \
                  __FILE__,                                                              \
                  __LINE__,                                                              \
                  #expression);                                                          \
              FLOW_DEBUG_BREAK();                                                        \
          }                                                                              \
      }                                                                                  \
      while (0)
// NOLINTEND
#else
#  define FLOW_ASSERT(expression, message, ...) (void)0
#endif

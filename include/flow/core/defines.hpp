#pragma once

#if defined(_MSC_VER)
#  define FLOW_COMPILER_MSVC 1
#elif defined(__GNUC__)
#  if defined(__clang__)
#    define FLOW_COMPILER_CLANG 1
#  else
#    define FLOW_COMPILER_GCC 1
#  endif
#else
#  error Unknown compiler
#endif

#if defined(FLOW_COMPILER_MSVC)
#  define FLOW_INLINE __forceinline
#elif defined(FLOW_COMPILER_CLANG) || defined(FLOW_COMPILER_GCC)
#  define FLOW_INLINE __attribute__((always_inline))
#else
#  define FLOW_INLINE inline
#endif

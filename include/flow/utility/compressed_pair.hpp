#pragma once

namespace flow {

#if FLOW_PLATFORM_WINDOWS
#  define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#  define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

template<typename T, typename U>
struct compressed_pair
{
    NO_UNIQUE_ADDRESS T first;
    NO_UNIQUE_ADDRESS U second;
};

#undef NO_UNIQUE_ADDRESS

} // namespace flow

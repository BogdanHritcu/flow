#pragma once

#include <string>
#include <string_view>

#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "serialization.hpp"

namespace flow {

template<typename CharT, typename Traits, typename AllocatorT>
struct deserializer<std::basic_string<CharT, Traits, AllocatorT>>
{
    void operator()(istream_view in, std::basic_string<CharT, Traits, AllocatorT>& str) const
    {
        using size_type = typename std::basic_string<CharT, Traits, AllocatorT>::size_type;

        size_type size;
        if (!in.read(size))
        {
            return;
        }
        str.resize(size);
        in.read(std::span{ str });
    }
};

template<typename CharT, typename Traits, typename AllocatorT>
struct serializer<std::basic_string<CharT, Traits, AllocatorT>>
{
    void operator()(ostream_view out, const std::basic_string<CharT, Traits, AllocatorT>& str) const
    {
        if (!out.write(str.size()))
        {
            return;
        }
        out.write(std::span{ str });
    }
};

template<typename CharT, typename Traits>
struct deserializer<std::basic_string_view<CharT, Traits>>
{
    void operator()(istream_view in, std::basic_string_view<CharT, Traits>& str) const
    {
        in.read(std::span{ str });
    }
};

template<typename CharT, typename Traits>
struct serializer<std::basic_string_view<CharT, Traits>>
{
    void operator()(ostream_view out, const std::basic_string_view<CharT, Traits>& str) const
    {
        out.write(std::span{ str });
    }
};

} // namespace flow

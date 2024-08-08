#pragma once

#include <utility>
#include <vector>

#include "concepts.hpp"
#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "serialization.hpp"

namespace flow {

template<typename T, typename AllocatorT>
struct deserializer<std::vector<T, AllocatorT>>
{
    void operator()(istream_view in, std::vector<T, AllocatorT>& v) const
    {
        using size_type = typename std::vector<T, AllocatorT>::size_type;

        size_type size;
        if (!in.read(size))
        {
            return;
        }

        if constexpr (concepts::trivially_copyable<T>)
        {
            v.resize(size);
            in.read(std::span{ v });
        }
        else
        {
            v.clear();

            for (size_type i = 0; i < size; ++i)
            {
                T e;
                if (!in.deserialize(e))
                {
                    return;
                }

                v.push_back(std::move(e));
            }
        }
    }
};

template<typename T, typename AllocatorT>
struct serializer<std::vector<T, AllocatorT>>
{
    void operator()(ostream_view out, const std::vector<T, AllocatorT>& v) const
    {
        if (!out.write(v.size()))
        {
            return;
        }

        if constexpr (concepts::trivially_copyable<T>)
        {
            out.write(std::span{ v });
        }
        else
        {
            for (const auto& e : v)
            {
                if (!out.serialize(e))
                {
                    return;
                }
            }
        }
    }
};

} // namespace flow

#pragma once

#include <unordered_map>
#include <utility>

#include "concepts.hpp"
#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "pair_serialization.hpp"
#include "serialization.hpp"

namespace flow {

template<typename KeyT,
         typename MappedT,
         typename HashT,
         typename EqT,
         typename AllocatorT>
struct deserializer<std::unordered_map<KeyT, MappedT, HashT, EqT, AllocatorT>>
{
    void operator()(istream_view in, std::unordered_map<KeyT, MappedT, HashT, EqT, AllocatorT>& m) const
    {
        using size_type = typename std::unordered_map<KeyT, MappedT, HashT, EqT, AllocatorT>::size_type;

        size_type size;
        if (!in.read(size))
        {
            return;
        }

        m.clear();
        m.reserve(size);

        for (size_type i = 0; i < size; ++i)
        {
            std::pair<KeyT, MappedT> p;
            if (!in.deserialize(p))
            {
                return;
            }

            m.insert(std::move(p));
        }
    }
};

template<typename KeyT,
         typename MappedT,
         typename HashT,
         typename EqT,
         typename AllocatorT>
struct serializer<std::unordered_map<KeyT, MappedT, HashT, EqT, AllocatorT>>
{
    void operator()(ostream_view out, const std::unordered_map<KeyT, MappedT, HashT, EqT, AllocatorT>& m) const
    {
        if (!out.write(m.size()))
        {
            return;
        }

        for (const auto& p : m)
        {
            if (!out.serialize(p))
            {
                return;
            }
        }
    }
};

} // namespace flow

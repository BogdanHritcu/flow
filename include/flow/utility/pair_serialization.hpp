#pragma once

#include <utility>

#include "compressed_pair.hpp"
#include "concepts.hpp"
#include "istream_view.hpp"
#include "ostream_view.hpp"
#include "serialization.hpp"

namespace flow {

template<template<typename, typename> typename PairT, typename T, typename U>
struct deserializer<PairT<T, U>>
{
    void operator()(istream_view in, PairT<T, U>& p) const
    {
        if constexpr (concepts::trivially_copyable<PairT<T, U>>)
        {
            in.read(p);
        }
        else
        {
            in.deserialize(p.first);
            in.deserialize(p.second);
        }
    }
};

template<template<typename, typename> typename PairT, typename T, typename U>
struct serializer<PairT<T, U>>
{
    void operator()(ostream_view out, const PairT<T, U>& p) const
    {
        if constexpr (concepts::trivially_copyable<PairT<T, U>>)
        {
            out.write(p);
        }
        else
        {
            out.serialize(p.first);
            out.serialize(p.second);
        }
    }
};

} // namespace flow

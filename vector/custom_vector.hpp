#pragma once

#include <memory>

namespace custom 
{
    template <typename T, typename Alloc = std::allocator<T>>
    class vector
    {

    };

    template <>
    class vector<void>
    {

    };
} // custom
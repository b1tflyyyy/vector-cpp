// MIT License

// Copyright (c) 2024 Andrii

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <initializer_list>

namespace custom 
{
    template <typename T, typename Alloc = std::allocator<T>>
    class vector
    {
    public:
        using value_type = T;
        using allocator_type = Alloc;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using allocator_traits = std::allocator_traits<Alloc>;
        using pointer = allocator_traits::pointer;
        using const_pointer = allocator_traits::const_pointer;
    
    public:
        // ----------------------------------------------------------------------------------------------------------------------
        class iterator
        {

        };

    public:
        // ----------------------------------------------------------------------------------------------------------------------
        constexpr vector() noexcept(noexcept(Alloc())) : 
            Capacity{ }, Size{ }, Buffer{ nullptr }
        { }

        // ----------------------------------------------------------------------------------------------------------------------
        explicit vector(size_type count, const_reference value, const Alloc& alloc = Alloc()) :
            Size{ count }, Capacity{ count * 2 }, Allocator{ alloc }
        { 
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::construct(Allocator, (Buffer + i), value);
            }
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr vector(std::initializer_list<value_type> list, const Alloc& alloc = Alloc()) :
            Size{ list.size() }, Capacity{ list.size() * 2 }, Allocator{ alloc }
        {
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            std::copy(std::begin(list), std::end(list), Buffer);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        vector(const vector& other) :
            Size{ other.Size }, Capacity{ other.Capacity }, Allocator{ other.Allocator }
        {
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            std::memcpy(Buffer, other.Buffer, sizeof(value_type) * Size);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        vector& operator=(const vector& other)
        {
            if (Capacity < other.Size)
            {
                this->~vector();
                Capacity = other.Capacity;

                Buffer = allocator_traits::allocate(Allocator, Capacity);
            }

            Size = other.Size;
            std::memcpy(Buffer, other.Buffer, sizeof(value_type) * Size);

            return *this;
        } 

        // ----------------------------------------------------------------------------------------------------------------------
        ~vector() noexcept
        {
            allocator_traits::deallocate(Allocator, Buffer, Capacity);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr pointer data() const noexcept
        {
            return Buffer;
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr bool is_empty() const noexcept
        {
            return !Size;
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr size_type size() const noexcept
        {
            return Size;
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr size_type capacity() const noexcept
        {
            return Capacity;
        }

    private:
        std::size_t Capacity;
        std::size_t Size;
        pointer Buffer;
        Alloc Allocator;    
    };

    template <>
    class vector<void>
    {

    };
} // custom
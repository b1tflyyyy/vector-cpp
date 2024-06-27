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
#include <cstdint>
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
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = allocator_traits::pointer;
            using reference = value_type&;

        public:    
            // ----------------------------------------------------------------------------------------------------------------------
            explicit iterator(pointer ptr) :
                Pointer{ ptr }
            { }

            ~iterator() = default;

            // ----------------------------------------------------------------------------------------------------------------------
            reference operator*() const
            {
                return *Pointer;
            }

            // ----------------------------------------------------------------------------------------------------------------------
            pointer operator->() 
            {
                return Pointer;
            }

            // ----------------------------------------------------------------------------------------------------------------------
            iterator& operator++()
            {
                ++Pointer;
                return *this;
            }

            // ----------------------------------------------------------------------------------------------------------------------
            iterator operator++(std::int32_t)
            {
                iterator tmp{ *this };
                ++(*this);

                return tmp;
            }

            // ----------------------------------------------------------------------------------------------------------------------
            bool operator==(const iterator& other) noexcept
            {
                return Pointer == other.Pointer;
            }

            // ----------------------------------------------------------------------------------------------------------------------
            bool operator!=(const iterator& other) noexcept
            {
                return Pointer != other.Pointer;
            }

        private:
            pointer Pointer;
        };

    public:
        // ----------------------------------------------------------------------------------------------------------------------
        constexpr vector() noexcept(noexcept(Alloc())) : 
            Capacity{ 2 }, Size{}, Buffer{ nullptr }
        { 
            Buffer = allocator_traits::allocate(Allocator, Capacity);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        explicit vector(size_type count, const_reference value, const Alloc& alloc = Alloc()) :
            Capacity{ count * 2 }, Size{ count }, Allocator{ alloc }
        { 
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::construct(Allocator, Buffer + i, value);
            }
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr vector(std::initializer_list<value_type> list, const Alloc& alloc = Alloc()) :
            Capacity{ list.size() * 2 }, Size{ list.size() }, Allocator{ alloc }
        {
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            std::uninitialized_copy(std::begin(list), std::end(list), Buffer);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        vector(const vector& other) :
            Capacity{ other.Capacity }, Size{ other.Size }, Allocator{ other.Allocator }
        {
            Buffer = allocator_traits::allocate(Allocator, Capacity);
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::construct(Allocator, Buffer + i, other.Buffer[i]);
            }
        }

        // ----------------------------------------------------------------------------------------------------------------------
        vector& operator=(const vector& other) 
        {
            if (this == &other)
            {
                return *this;
            }

            if (Capacity < other.Size)
            {
                this->~vector();
                Buffer = allocator_traits::allocate(Allocator, other.Capacity);

                Capacity = other.Capacity;
            }

            Size = other.Size;
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::construct(Allocator, Buffer + i, other.Buffer[i]);
            }

            return *this;
        } 

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr vector(vector&& other) noexcept
        {
            Size = other.Size;
            Capacity = other.Capacity;
            Buffer = other.Buffer;
            Allocator = std::move(other.Allocator);

            other.Size = 0;
            other.Capacity = 0;
            other.Buffer = nullptr;
            other.reserve(2); // TODO: think about it
        }

        // ---------------------------------------------------------------------------------------------------------------------- 
        vector& operator=(vector&& other) noexcept(allocator_traits::propagate_on_container_move_assignment::value ||
                                                   allocator_traits::is_always_equal::value)
        {
            if (!this->is_empty())
            {
                this->~vector();
            }

            Size = other.Size;
            Capacity = other.Capacity;
            Buffer = other.Buffer;
            Allocator = std::move(other.Allocator);

            other.Size = 0;
            other.Capacity = 0;
            other.Buffer = nullptr;
            other.reserve(2); // TODO: think about it

            return *this;
        }

        // ----------------------------------------------------------------------------------------------------------------------
        ~vector() noexcept
        {
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::destroy(Allocator, Buffer + i);
            }

            allocator_traits::deallocate(Allocator, Buffer, Capacity);
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr reference operator[](size_type idx)
        {
            return Buffer[idx];
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr const_reference operator[](size_type idx) const
        {
            return Buffer[idx];
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

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr iterator begin() noexcept
        {
            return iterator{ Buffer };
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr iterator end() noexcept
        {
            return iterator{ Buffer + Size };
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr void reserve(std::size_t new_capacity) 
        {
            if (new_capacity <= Capacity)
            {
                return;
            }

            pointer new_buffer{ allocator_traits::allocate(Allocator, new_capacity) };
            for (std::size_t i{}; i < Size; ++i)
            {
                allocator_traits::construct(Allocator, new_buffer + i, std::move(Buffer[i]));
            }

            this->~vector();
            Buffer = new_buffer;
            Capacity = new_capacity;
        }

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr void push_back(const_reference value)
        {
            if (Size == Capacity)
            {
                this->reserve(Capacity * 2);
            }

            allocator_traits::construct(Allocator, Buffer + Size, value);
            ++Size;
        } 

        // ----------------------------------------------------------------------------------------------------------------------
        constexpr void push_back(value_type&& value)
        {
            if (Size == Capacity)
            {
                this->reserve(Capacity * 2);
            }

            allocator_traits::construct(Allocator, Buffer + Size, std::move(value));
            ++Size;
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
// MIT License
// 
// Copyright (c) 2025 Mr. Myxa
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
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
#include <utility>
#include <algorithm>
#include <concepts>
#include <type_traits>

namespace vectorx 
{
    namespace detail
    {
        template <typename T, typename Alloc = std::allocator<T>>
        class Buffer
        {
        public:
            using alloc_traits = std::allocator_traits<Alloc>;

        public:
            constexpr Buffer() 
                : mAlloc{}
                , mBuffer{ nullptr }
                , mCapacity{}
            { }

            constexpr Buffer(const Alloc& alloc) 
                : mAlloc{ alloc }
                , mBuffer{ nullptr }
                , mCapacity{}
            { }

            explicit constexpr Buffer(std::size_t capacity, const Alloc& alloc = Alloc{}) 
                : mAlloc{ alloc }
                , mBuffer{ alloc_traits::allocate(mAlloc, capacity) }
                , mCapacity{ capacity }
            { }

            constexpr Buffer(const Buffer& rhs) 
                : mAlloc{ rhs.mAlloc }
                , mBuffer{ alloc_traits::allocate(mAlloc, rhs.mCapacity) }
                , mCapacity{ rhs.mCapacity }
            { }

            constexpr Buffer(Buffer&& rhs) noexcept 
                : mAlloc{ std::move(rhs.mAlloc) } // *
                , mBuffer{ std::exchange(rhs.mBuffer, nullptr) }
                , mCapacity{ std::exchange(rhs.mCapacity, 0) }
            { }

            constexpr Buffer& operator=(const Buffer& rhs)
            {
                if (this != &rhs)
                {
                    Buffer copy{ rhs };
                    swap(*this, copy);
                }

                return *this;
            }

            constexpr Buffer& operator=(Buffer&& rhs) noexcept
            {
                if (this != &rhs) // *
                {   
                    alloc_traits::deallocate(mAlloc, mBuffer, mCapacity);
                    mAlloc = std::move(rhs.mAlloc);

                    mBuffer = std::exchange(rhs.mBuffer, nullptr); 
                    mCapacity = std::exchange(rhs.mCapacity, 0);
                }

                return *this;
            }

            constexpr ~Buffer() noexcept
            {
                if (mCapacity != 0 && mBuffer != nullptr)
                {
                    alloc_traits::deallocate(mAlloc, mBuffer, mCapacity);
                }
            }

            constexpr T* data(std::size_t offset = 0) noexcept // *
            {
                return mBuffer + offset;
            }

            constexpr const T* data(std::size_t offset = 0) const noexcept // * constexpr 
            {
                return mBuffer + offset;
            }

            constexpr std::size_t capacity() const noexcept
            {
                return mCapacity;
            }

            constexpr Alloc& get_allocator()
            {
                return mAlloc;
            }

            friend void swap(Buffer& lhs, Buffer& rhs) noexcept
            {
                using std::swap;

                swap(lhs.mAlloc, rhs.mAlloc);
                swap(lhs.mBuffer, rhs.mBuffer);
                swap(lhs.mCapacity, rhs.mCapacity);
            }

        private:
            Alloc mAlloc;
            
            T* mBuffer;
            std::size_t mCapacity;
        };

        template <typename T, typename... Args>
        void uninitialized_construct_with_args_n(std::size_t n, T* location, Args&&... args)
        {
            std::size_t i{};

            try
            {
                for (; i < n; ++i)
                {
                    std::construct_at(location + i, std::forward<Args>(args)...);
                }
            }
            catch (...)
            {
                std::destroy_n(location, i);
                throw;
            }
        } 
    } // namespace detail

    template <typename T, typename Alloc = std::allocator<T>>
        requires std::is_nothrow_move_assignable_v<T> &&
                 std::is_nothrow_move_constructible_v<T>
    class vector
    {
    public:
        using value_type = T;
        using allocator_type = Alloc;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = typename std::allocator_traits<Alloc>::pointer;
        using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

        using buffer_t = detail::Buffer<T, Alloc>;

    public:
        // Nothrow
        constexpr vector() = default;

        // Nothrow if alloc nothrow
        constexpr vector(const Alloc& alloc)
            : mBuffer{ alloc }
            , mSize{}
        { }

        // Strong 
        constexpr vector(std::size_t capacity, const Alloc& alloc = Alloc{})
            : mBuffer{ capacity, alloc }
            , mSize{}
        { }

        // Strong
        constexpr vector(std::initializer_list<T> list, const Alloc& alloc = Alloc{}) 
            : mBuffer{ std::size(list), alloc }
            , mSize{}
        {
            const auto sz{ std::size(list) };

            std::uninitialized_copy_n(std::begin(list), sz, std::data(mBuffer));
            mSize = sz;
        }

        // Strong
        constexpr vector(const vector& rhs)
            : mBuffer{ rhs.mBuffer }
            , mSize{}
        {
            std::uninitialized_copy_n(std::data(rhs.mBuffer), rhs.mSize, std::data(mBuffer));
            mSize = rhs.mSize;
        }

        // Nothrow
        constexpr vector(vector&& rhs) noexcept
            : mBuffer{ std::move(rhs.mBuffer) }
            , mSize{ std::exchange(rhs.mSize, 0) }
        { }

        // Strong
        constexpr vector& operator=(const vector& rhs)
        {
            if (this != &rhs)
            {
                vector copy(rhs);
                swap(*this, copy);
            }

            return *this;
        }

        // Nothrow
        constexpr vector& operator=(vector&& rhs) noexcept // *
        {
            if (this != &rhs)
            {
                std::destroy_n(std::data(mBuffer), mSize);

                mSize = std::exchange(rhs.mSize, 0);
                mBuffer = std::move(rhs.mBuffer);
            }

            return *this;
        }

        // Nothrow
        constexpr ~vector() noexcept 
        {
            std::destroy_n(std::data(mBuffer), mSize);
        }

        // Nothrow
        constexpr size_type size() const noexcept { return mSize; }
        constexpr size_type capacity() const noexcept { return mBuffer.capacity(); }
        
        // Strong
        constexpr reference operator[](std::size_t index) { return *mBuffer.data(index); }
        constexpr const_reference operator[](std::size_t index) const { return *mBuffer.data(index); }

        // Nothrow
        constexpr pointer data() noexcept { return mBuffer.data(); };
        constexpr const_pointer data() const noexcept { return mBuffer.data(); }

        // Nothrow
        constexpr bool empty() const noexcept { return mSize == 0; }

        // Strong
        constexpr void reserve(size_type capacity)
        {
            if (mBuffer.capacity() >= capacity) { return; }

            vector copy(capacity, *this);
            swap(*this, copy);
        }

        // Strong
        constexpr void push_back(const T& value)
        {
            emplace_back(value);
        }

        // Strong
        constexpr void push_back(T&& value)
        {
            emplace_back(std::move(value));
        }

        // Strong
        template <typename... Args>
        constexpr reference emplace_back(Args&&... args)
        {
            if (auto cap{ capacity() }; cap == mSize)
            {
                cap = (cap == 0 ? 1 : cap * 2);

                vector copy(cap); 
                copy.construct_and_swap(*this, std::forward<Args>(args)...);
            }
            else 
            {
                std::construct_at(mBuffer.data(mSize), std::forward<Args>(args)... );
            }
            
            ++mSize;
            return *mBuffer.data(mSize - 1);
        }

        // Strong
        constexpr void resize(std::size_t new_sz)
        {
            if (new_sz == mSize) { return; }

            if (new_sz < mSize)
            {
                std::destroy_n(mBuffer.data(new_sz), mSize - new_sz);
            }
            else 
            {
                vector copy(new_sz * 2);
                copy.construct_and_swap_n(*this, new_sz - mSize);
            }

            mSize = new_sz;
        }

        // Strong
        constexpr void resize(std::size_t new_sz, const value_type& init_value)
        {
            if (new_sz == mSize) { return; }

            if (new_sz < mSize)
            {
                std::destroy_n(mBuffer.data(new_sz), mSize - new_sz);
            }
            else 
            {
                vector copy(new_sz * 2);
                copy.construct_and_swap_n(*this, new_sz - mSize, init_value);
            }

            mSize = new_sz;
        }

        friend bool operator==(const vector& lhs, const vector& rhs) noexcept
        {
            return std::equal(std::data(lhs), std::data(lhs) + std::size(lhs), std::data(rhs));
        }

        friend bool operator!=(const vector& lhs, const vector& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        friend void swap(vector& lhs, vector& rhs) noexcept
        {
            using std::swap;

            swap(lhs.mSize, rhs.mSize);
            swap(lhs.mBuffer, rhs.mBuffer);
        }

    private:
        constexpr vector(std::size_t capacity, vector& rhs)
            : mBuffer{ capacity } // move alloc or not ?
            , mSize{ std::size(rhs) }
        {
            std::uninitialized_move_n(std::data(rhs.mBuffer), std::size(rhs), std::data(mBuffer));
        }

        template <typename... Args> 
            requires (sizeof...(Args) >= 1) 
        constexpr void construct_and_swap(vector& other, Args&&... args)
        {
            const auto sz{ std::size(other) };

            std::construct_at(mBuffer.data(sz), std::forward<Args>(args)...);
            std::uninitialized_move_n(std::data(other.mBuffer), sz, std::data(mBuffer));
    
            mSize = other.mSize;
            swap(*this, other);
        }

        template <typename U = T>
            requires std::is_trivially_constructible_v<U>
        constexpr void construct_and_swap(vector& other)
        {
            const auto sz{ std::size(other) };

            std::construct_at(mBuffer.data(sz));
            std::uninitialized_move_n(std::data(other.mBuffer), sz, std::data(mBuffer));

            mSize = other.mSize;
            swap(*this, other);
        }

        template <typename... Args>
            requires (sizeof...(Args) >= 1)
        constexpr void construct_and_swap_n(vector& other, std::size_t n, Args&&... args)
        {
            const auto sz{ std::size(other) };

            detail::uninitialized_construct_with_args_n(n, mBuffer.data(sz), std::forward<Args>(args)...);
            std::uninitialized_move_n(std::data(other.mBuffer), sz, std::data(mBuffer));

            mSize = other.mSize;
            swap(*this, other);
        }

        template <typename U = T>
            requires std::is_trivially_constructible_v<U> 
        constexpr void construct_and_swap_n(vector& other, std::size_t n)
        {
            const auto sz{ std::size(other) };

            detail::uninitialized_construct_with_args_n(n, mBuffer.data(sz));
            std::uninitialized_move_n(std::data(other.mBuffer), sz, std::data(mBuffer));

            mSize = other.mSize;
            swap(*this, other);
        } 

    private:
        buffer_t mBuffer;
        size_type mSize;
    };

} // namespace vectorx

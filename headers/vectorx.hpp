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
            explicit Buffer(const Alloc& alloc = Alloc{}) 
                : mAlloc{ alloc }
                , mBuffer{ nullptr }
                , mCapacity{}
            { }

            explicit Buffer(std::size_t capacity, const Alloc& alloc = Alloc{}) 
                : mAlloc{ alloc }
                , mBuffer{ alloc_traits::allocate(mAlloc, capacity) }
                , mCapacity{ capacity }
            { }

            Buffer(const Buffer& rhs) 
                : mAlloc{ rhs.mAlloc }
                , mBuffer{ alloc_traits::allocate(mAlloc, rhs.mCapacity) }
                , mCapacity{ rhs.mCapacity }
            { }

            Buffer(Buffer&& rhs) noexcept 
                : mAlloc{ rhs.mAlloc }
                , mBuffer{ std::exchange(rhs.mBuffer, nullptr) }
                , mCapacity{ std::exchange(rhs.mCapacity, 0) }
            { }

            Buffer& operator=(const Buffer& rhs)
            {
                if (this != &rhs)
                {
                    Buffer copy{ rhs };
                    swap(*this, copy);
                }

                return *this;
            }

            Buffer& operator=(Buffer&& rhs) noexcept
            {
                if (this != &rhs)
                {
                    auto old_alloc{ mAlloc };
                    mAlloc = rhs.mAlloc;
                    
                    alloc_traits::deallocate(old_alloc, mBuffer, mCapacity);

                    mBuffer = std::exchange(rhs.mBuffer, nullptr); 
                    mCapacity = std::exchange(rhs.mCapacity, 0);
                }

                return *this;
            }

            ~Buffer() noexcept
            {
                if (mCapacity != 0 && mBuffer != nullptr)
                {
                    alloc_traits::deallocate(mAlloc, mBuffer, mCapacity);
                }
            }

            constexpr T* data(std::size_t offset = 0) noexcept
            {
                return mBuffer + offset;
            }

            constexpr const T* data(std::size_t offset = 0) const noexcept
            {
                return mBuffer + offset;
            }

            constexpr std::size_t capacity() const noexcept
            {
                return mCapacity;
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
    } // namespace detail

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
        using pointer = typename std::allocator_traits<Alloc>::pointer;
        using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

    public:
        // Nothrow if alloc nothrow
        vector(const Alloc& alloc = Alloc{})
            : mSize{}
            , mBuffer{ alloc }
        { }

        // Strong
        vector(std::initializer_list<T> list, const Alloc& alloc = Alloc{}) 
            : mSize{}
            , mBuffer{ std::size(list), alloc }
        {
            const auto sz{ std::size(list) };

            std::uninitialized_copy_n(std::begin(list), sz, std::data(mBuffer));
            mSize = sz;
        }

        // Strong
        vector(const vector& rhs)
            : mSize{}
            , mBuffer{ rhs.mBuffer }
        {
            std::uninitialized_copy_n(std::data(rhs.mBuffer), rhs.mSize, std::data(mBuffer));
            mSize = rhs.mSize;
        }

        // Nothrow
        vector(vector&& rhs) noexcept
            : mSize{ std::exchange(rhs.mSize, 0) }
            , mBuffer{ std::move(rhs.mBuffer) }
        { }

        // Strong
        vector& operator=(const vector& rhs)
        {
            if (this != &rhs)
            {
                vector copy{ rhs };
                swap(*this, copy);
            }

            return *this;
        }

        // Nothrow
        vector& operator=(vector&& rhs) noexcept
        {
            mSize = std::exchange(rhs.mSize, 0 );
            mBuffer = std::move(rhs.mBuffer);
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
        constexpr bool empty() const noexcept { return !mSize; }

        // Strong
        void reserve(size_type capacity)
        {
            if (mBuffer.capacity() >= capacity)
            {
                return;
            }

            decltype(mBuffer) new_buffer{ capacity }; // without alloc copy
            std::uninitialized_copy_n(std::data(mBuffer), mSize, std::data(new_buffer));

            swap(new_buffer, mBuffer);
        }

        // Strong
        constexpr void push_back(const T& value)
        {
            if (auto cap{ mBuffer.capacity() }; mSize == cap)
            {
                cap = (!cap ? 2u : cap * 2);
                
                decltype(mBuffer) new_buffer{ cap }; // without alloc copy
                std::uninitialized_copy_n(std::data(mBuffer), mSize, std::data(new_buffer));

                try 
                {
                    auto* ptr{ new_buffer.data(mSize) };
                    std::construct_at(ptr, value);
                }
                catch (...)
                {
                    std::destroy_n(std::data(new_buffer), mSize);
                    throw;
                }

                swap(mBuffer, new_buffer);
            }
            else 
            {
                auto* ptr{ mBuffer.data(mSize) };
                std::construct_at(ptr, value);
            }

            ++mSize;
        }

        // Strong
        constexpr void push_back(T&& value)
        {
            if (auto cap{ mBuffer.capacity() }; mSize == cap)
            {
                cap = (!cap ? 2u : cap * 2);

                decltype(mBuffer) new_buffer{ cap }; // without alloc copy
                std::uninitialized_copy_n(std::data(mBuffer), mSize, std::data(new_buffer));

                try 
                {
                    auto* ptr{ new_buffer.data(mSize) };
                    std::construct_at(ptr, std::move(value));
                }
                catch (...)
                {
                    std::destroy_n(std::data(new_buffer), mSize);
                    throw;
                }

                swap(mBuffer, new_buffer);
            }
            else 
            {
                auto* ptr{ mBuffer.data(mSize) };
                std::construct_at(ptr, std::move(value));
            }

            ++mSize;
        }

        template <typename... Args>
        constexpr reference emplace_back(Args&&... args)
        {
            if (auto cap{ mBuffer.capacity() }; mSize == cap)
            {
                cap = (!cap ? 2u : cap * 2);

                decltype(mBuffer) new_buffer{ cap }; // without alloc copy
                std::uninitialized_copy_n(std::data(mBuffer), mSize, std::data(new_buffer));

                try 
                {
                    auto* ptr{ new_buffer.data(mSize) };
                    std::construct_at(ptr, std::forward<Args>(args)...);
                }
                catch (...)
                {
                    std::destroy_n(std::data(new_buffer), mSize);
                    throw;
                }

                swap(mBuffer, new_buffer);
            }
            else 
            {
                auto* ptr{ mBuffer.data(mSize) };
                std::construct_at(ptr, std::forward<Args>(args)...);
            }

            ++mSize;
            return *mBuffer.data(mSize - 1);
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
        size_type mSize;
        detail::Buffer<T, Alloc> mBuffer;
    };

} // namespace vectorx

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
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <utility>

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

            T* data(std::size_t offset = 0) noexcept
            {
                return mBuffer + offset;
            }

            const T* data(std::size_t offset = 0) const noexcept
            {
                return mBuffer + offset;
            }

            std::size_t capacity() const noexcept
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

} // namespace vectorx

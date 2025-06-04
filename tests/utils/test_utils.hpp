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

#include <cstdint>
#include <stdexcept>
#include <new>
#include <iostream>
#include <format>
#include <utility>

namespace test_utils
{
    namespace thr_alloc
    {
        enum class ThrowOn : std::uint32_t
        {
            Copy,
            Alloc,
        };

        namespace detail
        {
            template <typename T>
            T* stub_ptr() { return reinterpret_cast<T*>(0xFF); }

            struct AllocStats
            {
                friend std::ostream& operator<<(std::ostream& os, const AllocStats& alloc_stats)
                {
                    os << std::format("[alloc stats -> allocs: {}, deallocs: {}, copies: {}]", alloc_stats.AllocCounter, alloc_stats.DeallocCounter, alloc_stats.CopyCounter);
                    return os;
                }

                std::size_t AllocCounter{};
                std::size_t DeallocCounter{};
                std::size_t CopyCounter{};
            };

            template <typename T, ThrowOn throwOn>
            struct ThrowingAllocator
            {
                using value_type = T;

                ThrowingAllocator(AllocStats& stats, bool& can_throw) 
                    : mStats{ stats }
                    , mCanThrow{ can_throw } 
                { };
                
                ThrowingAllocator(const ThrowingAllocator& rhs) 
                    : mStats{ rhs.mStats } 
                    , mCanThrow{ rhs.mCanThrow } 
                { 
                    if (throwOn == ThrowOn::Copy && mCanThrow)
                    {
                        throw std::runtime_error{ "" };
                    }

                    ++mStats.CopyCounter;
                };
                
                ThrowingAllocator& operator=(const ThrowingAllocator& rhs)
                {
                    if (this != &rhs)
                    {
                        if (throwOn == ThrowOn::Copy && mCanThrow)
                        {
                            throw std::runtime_error{ "" };
                        }

                        mStats = rhs.mStats;
                        ++mStats.CopyCounter;
                    }

                    return *this;
                }

                T* allocate([[maybe_unused]] std::size_t n) 
                {
                    if (throwOn == ThrowOn::Alloc && mCanThrow)
                    {
                        throw std::bad_alloc{}; 
                    }

                    ++mStats.AllocCounter;
                    return stub_ptr<T>();
                }
                
                void deallocate([[maybe_unused]] T* ptr, [[maybe_unused]] std::size_t n) { ++mStats.DeallocCounter; }

                AllocStats& mStats;
                bool& mCanThrow;
            };
        } // namespace detail

        template <typename T, ThrowOn throwOn>
        struct ThrowingAllocator
        {
            using internal_alloc_t = detail::ThrowingAllocator<T, throwOn>;

            ThrowingAllocator(bool can_throw) 
                : mCanThrow{ can_throw }
                , mStats{}
                , mInternalAlloc{ mStats, mCanThrow }
            { }

            bool mCanThrow;
            detail::AllocStats mStats;
            detail::ThrowingAllocator<T, throwOn> mInternalAlloc;
        };
    } // namespace thr_alloc

    namespace thr_object
    {
        enum class ThrowPolicy
        {
            ThrowOnCopy
        };

        namespace detail
        {
            struct ObjectStats
            {
                friend std::ostream& operator<<(std::ostream& os, const ObjectStats& stats)
                {
                    os << std::format("[object stats -> ctor: {}, dtor: {}, copy: {}]", stats.CtorCounter, stats.DtorCounter, stats.CopyCounter);
                    return os;
                }

                friend bool operator==(const ObjectStats& lhs, const ObjectStats& rhs)
                {
                    return lhs.CtorCounter == rhs.CtorCounter &&
                           lhs.CopyCounter == rhs.CopyCounter &&
                           lhs.MoveCounter == rhs.MoveCounter &&
                           lhs.DtorCounter == rhs.DtorCounter;
                }

                std::size_t CtorCounter;
                std::size_t DtorCounter;
                std::size_t CopyCounter;
                std::size_t MoveCounter;
            };

            struct ThrowObject
            {
                ThrowObject(ObjectStats& stats, 
                            ThrowPolicy policy, 
                            bool& can_throw, 
                            std::int32_t magic_value) 
                    : mPolicy{ policy }
                    , mStats{ stats }
                    , mCanThrow{ can_throw }
                    , mMagicValue{ magic_value }
                { 
                    ++mStats.CtorCounter;
                }

                ThrowObject(const ThrowObject& rhs) 
                    : mPolicy{ rhs.mPolicy }
                    , mStats{ rhs.mStats }
                    , mCanThrow{ rhs.mCanThrow }
                    , mMagicValue{ rhs.mMagicValue }
                {
                    if (mPolicy == ThrowPolicy::ThrowOnCopy && mCanThrow)
                    {
                        throw std::runtime_error{ "" };
                    }

                    ++mStats.CopyCounter;
                }

                ThrowObject& operator=(const ThrowObject& rhs)
                {
                    if (mPolicy == ThrowPolicy::ThrowOnCopy && mCanThrow)
                    {
                        throw std::runtime_error{ "" };
                    }

                    if (this != &rhs)
                    {
                        mCanThrow = rhs.mCanThrow;
                        mMagicValue = rhs.mMagicValue;
                        mPolicy = rhs.mPolicy;

                        mStats = rhs.mStats;
                        ++mStats.CopyCounter;
                    }

                    return *this;
                }

                ThrowObject(ThrowObject&& rhs) noexcept 
                    : mPolicy{ rhs.mPolicy }
                    , mStats{ rhs.mStats }
                    , mCanThrow{ rhs.mCanThrow }
                    , mMagicValue{ rhs.mMagicValue }
                {
                    ++mStats.MoveCounter;
                }

                ThrowObject& operator=(ThrowObject&& rhs) noexcept
                {
                    if (this != &rhs)
                    {
                        mCanThrow = rhs.mCanThrow;
                        mMagicValue = rhs.mMagicValue;
                        mPolicy = rhs.mPolicy;

                        mStats = rhs.mStats;
                        ++mStats.MoveCounter;
                    }

                    return *this;
                }

                ~ThrowObject() noexcept
                {
                    ++mStats.DtorCounter;
                }

                ThrowPolicy mPolicy;
                ObjectStats& mStats;
                bool& mCanThrow;
                std::int32_t mMagicValue;
            };
        } // namespace detail

        template <ThrowPolicy throwPolicy>
        struct ThrowObject
        {
            using internal_obj_t = detail::ThrowObject;

            ThrowObject(bool can_throw, std::int32_t magic_value) 
                : mCanThrow{ can_throw }
                , mStats{}
                , mInternalObject{ mStats, throwPolicy, mCanThrow, magic_value }
            { }

            bool mCanThrow;
            detail::ObjectStats mStats; 
            detail::ThrowObject mInternalObject;
        };
    } // namespace thr_object

    namespace nothrow_object
    {
        struct NothrowObjectWithAllocs
        {
            NothrowObjectWithAllocs(std::int32_t magic_value) 
                : mPtr{ new std::int32_t(magic_value) }
            { }

            NothrowObjectWithAllocs(const NothrowObjectWithAllocs& rhs)
                : mPtr{ new std::int32_t(*rhs.mPtr) }
            { }

            NothrowObjectWithAllocs& operator=(const NothrowObjectWithAllocs&) = default;

            NothrowObjectWithAllocs(NothrowObjectWithAllocs&& rhs) noexcept 
                : mPtr{ std::exchange(rhs.mPtr, nullptr) }
            { }

            NothrowObjectWithAllocs& operator=(NothrowObjectWithAllocs&&) noexcept = default;

            ~NothrowObjectWithAllocs() noexcept { delete mPtr; }

            std::int32_t value() const noexcept { return *mPtr; }

        private:
            std::int32_t* mPtr{}; 
        };
    } // namespace nothrow_object
}
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

        enum class ThrowOn : std::uint32_t
        {
            Copy,
            Alloc,
            CopyControl,
            AllocControl
        };

        template <typename T, ThrowOn throwOn>
        struct ThrowingAllocator;

        template <typename T>
        struct ThrowingAllocator<T, ThrowOn::Alloc>
        {
            using value_type = T;

            ThrowingAllocator(AllocStats& stats) : mStats{ stats } { };
            ThrowingAllocator(const ThrowingAllocator& rhs) : mStats{ rhs.mStats } { };
            ThrowingAllocator& operator=(const ThrowingAllocator& rhs)
            {
                if (this != &rhs)
                {
                    mStats = rhs.mStats;
                    ++mStats.CopyCounter;
                }

                return *this;
            }

            T* allocate([[maybe_unused]] std::size_t n) { throw std::bad_alloc{}; }
            void deallocate([[maybe_unused]] T* ptr, [[maybe_unused]] std::size_t n) { ++mStats.DeallocCounter; }

            AllocStats& mStats;
        };

        template <typename T>
        struct ThrowingAllocator<T, ThrowOn::Copy>
        {
            using value_type = T;

            ThrowingAllocator(AllocStats& stats) : mStats{ stats } { };
            ThrowingAllocator([[maybe_unused]] const ThrowingAllocator& rhs) : mStats{ rhs.mStats } { throw std::runtime_error{ "" }; };
            ThrowingAllocator& operator=(const ThrowingAllocator& rhs)
            {
                if (this != &rhs)
                {
                    mStats = rhs.mStats;
                    throw std::runtime_error{ "" };
                }

                return *this;
            }

            T* allocate([[maybe_unused]] std::size_t n) { ++mStats.AllocCounter; return stub_ptr<T>(); }
            void deallocate([[maybe_unused]] T* ptr, [[maybe_unused]] std::size_t n) { ++mStats.DeallocCounter; }
        
            AllocStats& mStats;
        };

        template <typename T>
        struct ThrowingAllocator<T, ThrowOn::CopyControl>
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
                if (mCanThrow)
                {
                    throw std::runtime_error{ "" }; 
                }

                ++mStats.CopyCounter;
            };

            ThrowingAllocator& operator=(const ThrowingAllocator& rhs)
            {
                if (this != &rhs)
                {
                    if (mCanThrow)
                    {
                        throw std::runtime_error{ "" };
                    }

                    mStats = rhs.mStats;
                    ++mStats.CopyCounter;
                }

                return *this;
            }
        
            T* allocate([[maybe_unused]] std::size_t n) { ++mStats.AllocCounter; return stub_ptr<T>(); }
            void deallocate([[maybe_unused]] T* ptr, [[maybe_unused]] std::size_t n) { ++mStats.DeallocCounter; }
        
            AllocStats& mStats;
            bool& mCanThrow;
        };

        template <typename T>
        struct ThrowingAllocator<T, ThrowOn::AllocControl>
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
                ++mStats.CopyCounter;
            };

            ThrowingAllocator& operator=(const ThrowingAllocator& rhs)
            {
                if (this != &rhs)
                {
                    mStats = rhs.mStats;
                    ++mStats.CopyCounter;
                }

                return *this;
            }
        
            T* allocate([[maybe_unused]] std::size_t n) 
            {
                if (mCanThrow)
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
    } // namespace thr_alloc

    namespace thr_object
    {
        enum class ThrowPolicy
        {
            ThrowOnCopy,
            NoThrow
        };

        struct ObjectStats
        {
            friend std::ostream& operator<<(std::ostream& os, const ObjectStats& stats)
            {
                os << std::format("[object stats -> ctor: {}, dtor: {}, copy: {}]", stats.CtorCounter, stats.DtorCounter, stats.CopyCounter);
                return os;
            }

            std::size_t CtorCounter;
            std::size_t DtorCounter;
            std::size_t CopyCounter;
        };

        struct ThrowObject
        {
            ThrowObject(ObjectStats& stats, ThrowPolicy policy, bool& can_throw, std::int32_t magic_value = 0) 
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

            ~ThrowObject() noexcept
            {
                ++mStats.DtorCounter;
            }

            ThrowPolicy mPolicy;
            ObjectStats& mStats;
            bool& mCanThrow;
            std::int32_t mMagicValue;
        };
    } // namespace thr_object

    namespace nothrow_object
    {
        struct NothrowObject
        {
            NothrowObject() : CopyCounter{ 1 } {};

            NothrowObject(const NothrowObject& rhs) : CtorCounter{ rhs.CopyCounter + 1 } {}
            NothrowObject& operator=(const NothrowObject& rhs)
            {
                if (this != &rhs)
                {
                    CopyCounter = rhs.CopyCounter + 1;
                }
                
                return *this;
            }  

            NothrowObject(NothrowObject&& rhs) noexcept : MoveCounter{ rhs.MoveCounter + 1 } {}
            NothrowObject& operator=(NothrowObject&& rhs) noexcept
            {
                if (this != &rhs)
                {   
                    MoveCounter = rhs.MoveCounter + 1;
                }

                return *this;
            }

            ~NothrowObject() noexcept { ++DtorCounter; }

            friend std::ostream& operator<<(std::ostream& os, const NothrowObject& nothrow_obj)
            {
                os << std::format("[nothrow object -> ctor: {}, copy: {}, move: {}, dtor: {}]", nothrow_obj.CtorCounter, nothrow_obj.CopyCounter, nothrow_obj.MoveCounter, nothrow_obj.DtorCounter);
                return os;
            }

            friend bool operator==(const NothrowObject& lhs, const NothrowObject& rhs) noexcept
            {
                return    lhs.CtorCounter == rhs.CtorCounter 
                       && lhs.CopyCounter == rhs.CopyCounter
                       && lhs.MoveCounter == rhs.MoveCounter
                       && lhs.DtorCounter == rhs.DtorCounter;
            }

            std::size_t CtorCounter{};
            std::size_t CopyCounter{};
            std::size_t MoveCounter{};
            std::size_t DtorCounter{};
        };

        struct NothrowObjectWithAllocs
        {
            NothrowObjectWithAllocs(std::int32_t magic_value) 
                : mPtr{ new std::int32_t(magic_value) }
            { }

            NothrowObjectWithAllocs(const NothrowObjectWithAllocs& rhs)
                : mPtr{ new std::int32_t(*rhs.mPtr) }
            { }

            NothrowObjectWithAllocs& operator=(const NothrowObjectWithAllocs&) = delete;

            NothrowObjectWithAllocs(NothrowObjectWithAllocs&& rhs) noexcept 
                : mPtr{ std::exchange(rhs.mPtr, nullptr) }
            { }

            NothrowObjectWithAllocs& operator=(NothrowObjectWithAllocs&&) noexcept = delete;

            ~NothrowObjectWithAllocs() noexcept { delete mPtr; }

            std::int32_t value() const noexcept { return *mPtr; }

        private:
            std::int32_t* mPtr{}; 
        };
    } // namespace nothrow_object
}
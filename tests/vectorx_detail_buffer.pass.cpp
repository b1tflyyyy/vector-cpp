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

#include <gtest/gtest.h>

#include "../headers/vectorx.hpp"
#include "utils/test_utils.hpp"

using namespace vectorx::detail;
using namespace test_utils::thr_alloc;

TEST(BufferTest, DefaultConstructor) 
{
    Buffer<int> buf;

    EXPECT_EQ(buf.capacity(), 0u);
    EXPECT_EQ(buf.data(), nullptr);
}

TEST(BufferTest, CapacityConstructor) 
{
    constexpr std::size_t N{ 10 };
    Buffer<int> buf{ N };
    
    EXPECT_EQ(buf.capacity(), N);
    EXPECT_NE(buf.data(), nullptr);
}

TEST(BufferTest, CopyConstructor) 
{
    constexpr std::size_t N{ 5 };
    
    Buffer<int> original{ N };
    int* orig_ptr{ original.data() };

    Buffer<int> copy{ original };
    
    EXPECT_EQ(copy.capacity(), original.capacity());
    EXPECT_NE(copy.data(), nullptr);
    EXPECT_NE(copy.data(), orig_ptr);
}

TEST(BufferTest, MoveConstructor) 
{
    constexpr std::size_t N{ 7 };
    
    Buffer<int> source{ N };
    int* source_ptr{ source.data() };

    Buffer<int> moved{ std::move(source) };

    EXPECT_EQ(moved.capacity(), N);
    EXPECT_EQ(moved.data(), source_ptr);
    EXPECT_EQ(source.capacity(), 0u);
    EXPECT_EQ(source.data(), nullptr);
}

TEST(BufferTest, CopyAssignment)
{
    Buffer<int> a{ 3 };
    Buffer<int> b{ 6 };
    int* b_ptr_before{ b.data() };

    a = b;
    EXPECT_EQ(a.capacity(), 6u);
    EXPECT_NE(a.data(), nullptr);
    EXPECT_NE(a.data(), b_ptr_before);

    a = a;
    EXPECT_EQ(a.capacity(), 6u);
}

TEST(BufferTest, MoveAssignment) 
{
    Buffer<int> a{ 4 };

    Buffer<int> b{ 8 };
    int* b_ptr_before{ b.data() };

    a = std::move(b);
    EXPECT_EQ(a.capacity(), 8u);
    EXPECT_EQ(a.data(), b_ptr_before);
    EXPECT_EQ(b.capacity(), 0u);
    EXPECT_EQ(b.data(), nullptr);

    a = std::move(a);
    EXPECT_EQ(a.capacity(), 8u);
    EXPECT_EQ(a.data(), b_ptr_before);
}

TEST(BufferTest, Swap) 
{
    Buffer<int> a{ 2 };
    Buffer<int> b{ 5 };
    int* a_ptr{ a.data() };
    int* b_ptr{ b.data() };

    swap(a, b);

    EXPECT_EQ(a.capacity(), 5u);
    EXPECT_EQ(a.data(), b_ptr);
    
    EXPECT_EQ(b.capacity(), 2u);
    EXPECT_EQ(b.data(), a_ptr);
}

TEST(BufferTest, ThrowOnAllocation)
{
    using T = ThrowingAllocator<int, ThrowOn::Alloc>; 
    T alloc{ true };
    
    try
    {
        Buffer<int, T::internal_alloc_t> buffer{ 10, alloc.mInternalAlloc };
        FAIL() << "exception expected";
    }
    catch (const std::bad_alloc& e) {}

    EXPECT_EQ(alloc.mStats.AllocCounter, 0);
    EXPECT_EQ(alloc.mStats.DeallocCounter, 0);
    EXPECT_EQ(alloc.mStats.CopyCounter, 1);
}

TEST(BufferTest, BufferCopyThrowOnCopyAllocator)
{
    using T = ThrowingAllocator<int, ThrowOn::Copy>; 
    T alloc{ true };

    try
    {
        Buffer<int, T::internal_alloc_t> buffer{ 10, alloc.mInternalAlloc };
        auto copy{ buffer };
    
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e) {}

    EXPECT_EQ(alloc.mStats.AllocCounter, 0);
    EXPECT_EQ(alloc.mStats.DeallocCounter, 0);
    EXPECT_EQ(alloc.mStats.CopyCounter, 0);
}

TEST(BufferTest, ThrowOnBufferCopyAllocatorCopyException)
{
    using T = ThrowingAllocator<int, ThrowOn::Copy>; 
    T alloc{ false };

    try
    {   
        Buffer<int, T::internal_alloc_t> buffer{ 10, alloc.mInternalAlloc };
        alloc.mCanThrow = true;

        auto copy{ buffer };
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e) {}

    EXPECT_EQ(alloc.mStats.AllocCounter, 1);
    EXPECT_EQ(alloc.mStats.DeallocCounter, 1);
    EXPECT_EQ(alloc.mStats.CopyCounter, 1);
}

TEST(BufferTest, ThrowOnBufferCopyAllocateException)
{
    using T = ThrowingAllocator<int, ThrowOn::Alloc>; 
    T alloc{ false  };
    
    try
    {
        Buffer<int, T::internal_alloc_t> buffer{ 10, alloc.mInternalAlloc };
        alloc.mCanThrow = true;

        auto copy{ buffer };
        FAIL() << "exception expected";
    }
    catch (const std::bad_alloc& e) {}

    EXPECT_EQ(alloc.mStats.AllocCounter, 1);
    EXPECT_EQ(alloc.mStats.DeallocCounter, 1);
    EXPECT_EQ(alloc.mStats.CopyCounter, 2);
}

TEST(BufferTest, InvariantsAfterCopyThrow) 
{
    using T = ThrowingAllocator<int, ThrowOn::Copy>; 
    T alloc{ false };

    Buffer<int, T::internal_alloc_t> buf{ 5, alloc.mInternalAlloc };
    
    int* old_ptr{ buf.data() };
    std::size_t old_cap{ buf.capacity() };

    try 
    {
        alloc.mCanThrow = true;
        auto tmp{ buf };

        FAIL() << "exception expected";
    }
    catch (const std::runtime_error&) {}

    EXPECT_EQ(buf.data(), old_ptr);
    EXPECT_EQ(buf.capacity(), old_cap);
}

TEST(BufferTest, InvariantsAfterCopyAssignmentThrow) 
{
    using T = ThrowingAllocator<int, ThrowOn::Copy>; 
    using U = Buffer<int, T::internal_alloc_t>;
    
    T alloc{ false };

    U a{ 5, alloc.mInternalAlloc };
    U b{ 7, alloc.mInternalAlloc };
    
    int* a_old_ptr{ a.data() };
    std::size_t a_old_cap{ a.capacity() };

    try 
    {
        alloc.mCanThrow = true;
        a = b;

        FAIL() << "exception expected";
    }
    catch (const std::runtime_error&) {}

    EXPECT_EQ(a.data(), a_old_ptr);
    EXPECT_EQ(a.capacity(), a_old_cap);
}
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
    AllocStats stats{};

    using ThrAlloc = ThrowingAllocator<int, ThrowOn::Alloc>; 
    ThrAlloc throwing_allocator{ stats };
    
    try
    {
        Buffer<int, ThrAlloc> buffer{ 10, throwing_allocator };
    }
    catch (const std::bad_alloc& e)
    {
        EXPECT_EQ(stats.AllocCounter, 0);
        EXPECT_EQ(stats.DeallocCounter, 0);
        EXPECT_EQ(stats.CopyCounter, 0);

        std::cout << throwing_allocator.mStats << '\n';
    }
}

TEST(BufferTest, BufferCopyThrowOnCopyAllocator)
{
    AllocStats stats{};

    using ThrAlloc = ThrowingAllocator<int, ThrowOn::Copy>; 
    ThrAlloc throwing_allocator{ stats };

    try
    {
        Buffer<int, ThrAlloc> buffer{ 10, throwing_allocator };
        auto copy{ buffer };
    }
    catch (const std::runtime_error& e)
    {
        EXPECT_EQ(stats.AllocCounter, 0);
        EXPECT_EQ(stats.DeallocCounter, 0);
        EXPECT_EQ(stats.CopyCounter, 0);

        std::cout << throwing_allocator.mStats << '\n';
    }
}

TEST(BufferTest, ThrowOnBufferCopyAllocatorCopyException)
{
    AllocStats stats{};
    bool can_throw{ false };

    try
    {
        using ThrAlloc = ThrowingAllocator<int, ThrowOn::CopyControl>;
        ThrAlloc throwing_allocator{ stats, can_throw };
        
        Buffer<int, ThrAlloc> buffer{ 10, throwing_allocator };
        
        can_throw = true;
        auto copy{ buffer };
    }
    catch (const std::runtime_error& e)
    {
        EXPECT_EQ(stats.AllocCounter, 1);
        EXPECT_EQ(stats.DeallocCounter, 1);
        EXPECT_EQ(stats.CopyCounter, 1);

        std::cout << stats << '\n';
    }
}

TEST(BufferTest, ThrowOnBufferCopyAllocateException)
{
    AllocStats stats{};
    bool can_throw{ false };

    try
    {
        using ThrAlloc = ThrowingAllocator<int, ThrowOn::AllocControl>;
        ThrAlloc throwing_allocator{ stats, can_throw };
        
        Buffer<int, ThrAlloc> buffer{ 10, throwing_allocator };
        
        can_throw = true;
        auto copy{ buffer };
    }
    catch (const std::bad_alloc& e)
    {
        EXPECT_EQ(stats.AllocCounter, 1);
        EXPECT_EQ(stats.DeallocCounter, 1);
        EXPECT_EQ(stats.CopyCounter, 2);

        std::cout << stats << '\n';
    }
}

TEST(BufferTest, InvariantsAfterCopyThrow) 
{
    AllocStats stats{};
    bool can_throw{ false };

    using ThrAlloc = ThrowingAllocator<int, ThrowOn::AllocControl>;
    ThrAlloc alloc{ stats, can_throw };

    Buffer<int, ThrAlloc> buf{ 5, alloc };
    
    int* old_ptr{ buf.data() };
    std::size_t old_cap{ buf.capacity() };

    try 
    {
        can_throw = true;
        Buffer<int, ThrAlloc> tmp{ buf };
    }
    catch (const std::bad_alloc&) {}

    EXPECT_EQ(buf.data(), old_ptr);
    EXPECT_EQ(buf.capacity(), old_cap);
}

TEST(BufferTest, InvariantsAfterCopyAssignmentThrow) 
{
    AllocStats stats{};
    bool can_throw{ false };
    
    using ThrAlloc = ThrowingAllocator<int, ThrowOn::AllocControl>;
    ThrAlloc alloc{ stats, can_throw };

    Buffer<int, ThrAlloc> a{ 5, alloc };
    Buffer<int, ThrAlloc> b{ 7, alloc };
    
    int* a_old_ptr{ a.data() };
    std::size_t a_old_cap{ a.capacity() };

    try 
    {
        can_throw = true;
        a = b;
    }
    catch (const std::bad_alloc&) {}

    EXPECT_EQ(a.data(), a_old_ptr);
    EXPECT_EQ(a.capacity(), a_old_cap);
}
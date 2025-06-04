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

using namespace test_utils::thr_object;
using namespace test_utils::nothrow_object;

TEST(VectorX, CtorNothrow)
{
    vectorx::vector<int> vec{ 1, 2, 3, 4, 5, 6 };
    for (std::size_t i{}; i < std::size(vec); ++i)
    {
        EXPECT_EQ(vec[i], i + 1);
    }
}

TEST(VectorX, CtorThrow)
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T o1{ false, 0 };
    T o2{ false, 0 };
    T o3{ false, 0 };
    T o4{ true,  0 };

    try
    {
        vectorx::vector<T::internal_obj_t> vec
        { 
            { 
                o1.mInternalObject,
                o2.mInternalObject,
                o3.mInternalObject,
                o4.mInternalObject,
            } 
        };

        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e) {}

    EXPECT_EQ(o1.mStats.CtorCounter, 1);
    EXPECT_EQ(o1.mStats.CopyCounter, 1);
    EXPECT_EQ(o1.mStats.DtorCounter, 1);

    EXPECT_EQ(o2.mStats.CtorCounter, 1);
    EXPECT_EQ(o2.mStats.CopyCounter, 1);
    EXPECT_EQ(o2.mStats.DtorCounter, 1);

    EXPECT_EQ(o3.mStats.CtorCounter, 1);
    EXPECT_EQ(o3.mStats.CopyCounter, 1);
    EXPECT_EQ(o3.mStats.DtorCounter, 1);

    EXPECT_EQ(o4.mStats.CtorCounter, 1);
    EXPECT_EQ(o4.mStats.CopyCounter, 0);
    EXPECT_EQ(o4.mStats.DtorCounter, 0);
}

TEST(VectorX, CopyCtorThrow)
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T oa1{ false, 1 };
    T oa2{ false, 2 };
    T oa3{ false, 3 };
    T oa4{ false, 4 };

    T ob1{ false, 11 };
    T ob2{ false, 22 };
    T ob3{ false, 33 };
    T ob4{ false, 44 };

    vectorx::vector<T::internal_obj_t> veca{ oa1.mInternalObject, oa2.mInternalObject, oa3.mInternalObject, oa4.mInternalObject };
    vectorx::vector<T::internal_obj_t> vecb{ ob1.mInternalObject, ob2.mInternalObject, ob3.mInternalObject, ob4.mInternalObject };

    try
    {
        oa4.mCanThrow = true;
        vecb = veca;

        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e) { }
    
    // A
    EXPECT_EQ(oa1.mStats.CtorCounter, 1);
    EXPECT_EQ(oa1.mStats.CopyCounter, 3);
    EXPECT_EQ(oa1.mStats.DtorCounter, 2);

    EXPECT_EQ(oa2.mStats.CtorCounter, 1);
    EXPECT_EQ(oa2.mStats.CopyCounter, 3);
    EXPECT_EQ(oa2.mStats.DtorCounter, 2);

    EXPECT_EQ(oa3.mStats.CtorCounter, 1);
    EXPECT_EQ(oa3.mStats.CopyCounter, 3);
    EXPECT_EQ(oa3.mStats.DtorCounter, 2);

    EXPECT_EQ(oa4.mStats.CtorCounter, 1);
    EXPECT_EQ(oa4.mStats.CopyCounter, 2);
    EXPECT_EQ(oa4.mStats.DtorCounter, 1);

    // B
    EXPECT_EQ(ob1.mStats.CtorCounter, 1);
    EXPECT_EQ(ob1.mStats.CopyCounter, 2);
    EXPECT_EQ(ob1.mStats.DtorCounter, 1);

    EXPECT_EQ(ob2.mStats.CtorCounter, 1);
    EXPECT_EQ(ob2.mStats.CopyCounter, 2);
    EXPECT_EQ(ob2.mStats.DtorCounter, 1);

    EXPECT_EQ(ob3.mStats.CtorCounter, 1);
    EXPECT_EQ(ob3.mStats.CopyCounter, 2);
    EXPECT_EQ(ob3.mStats.DtorCounter, 1);

    EXPECT_EQ(ob4.mStats.CtorCounter, 1);
    EXPECT_EQ(ob4.mStats.CopyCounter, 2);
    EXPECT_EQ(ob4.mStats.DtorCounter, 1);

    EXPECT_EQ(veca[0].mMagicValue, 1);
    EXPECT_EQ(veca[1].mMagicValue, 2);
    EXPECT_EQ(veca[2].mMagicValue, 3);
    EXPECT_EQ(veca[3].mMagicValue, 4);

    EXPECT_EQ(vecb[0].mMagicValue, 11);
    EXPECT_EQ(vecb[1].mMagicValue, 22);
    EXPECT_EQ(vecb[2].mMagicValue, 33);
    EXPECT_EQ(vecb[3].mMagicValue, 44);
}

TEST(VectorX, PushBackThrow) 
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;
    
    T o1{ false, 1 };
    T o2{ false, 2 };
    T o3{ true, 3 };
    
    vectorx::vector<T::internal_obj_t> vec{};
    
    vec.push_back(o1.mInternalObject);
    vec.push_back(o2.mInternalObject);
    
    try 
    {
        vec.push_back(o3.mInternalObject);
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error&) {}
    
    EXPECT_EQ(o1.mStats.CtorCounter, 1);
    EXPECT_EQ(o1.mStats.CopyCounter, 1); // was copied via push_back
    EXPECT_EQ(o1.mStats.MoveCounter, 1); // moved due to reallocs
    EXPECT_EQ(o1.mStats.DtorCounter, 1); // because of dtor of moved obj

    EXPECT_EQ(o2.mStats.CtorCounter, 1);
    EXPECT_EQ(o2.mStats.CopyCounter, 1); // was copied via push_back
    EXPECT_EQ(o2.mStats.MoveCounter, 0); // wasn't moved 
    EXPECT_EQ(o2.mStats.DtorCounter, 0); // wasn't moved

    EXPECT_EQ(o3.mStats.CtorCounter, 1);
    EXPECT_EQ(o3.mStats.CopyCounter, 0);
    EXPECT_EQ(o3.mStats.MoveCounter, 0);
    EXPECT_EQ(o3.mStats.DtorCounter, 0); // was not constructed via push_back

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 2);
    EXPECT_EQ(vec[0].mMagicValue, 1);
    EXPECT_EQ(vec[1].mMagicValue, 2);    
}

// deprecated due to move on reallocs
// TEST(VectorX, ReserveCopyThrow) 
// {
//     using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;
// 
//     T o1{ false, 1 };
//     T o2{ false, 2 };
// 
//     vectorx::vector<T::internal_obj_t> vec{};
// 
//     vec.reserve(2);
// 
//     vec.push_back(o1.mInternalObject);
//     vec.push_back(o2.mInternalObject);
// 
//     o2.mCanThrow = true;
//     auto* old_data{ vec.data() };
// 
//     try 
//     {
//         vec.reserve(128);
//         FAIL() << "exception expected";
//     }
//     catch (const std::runtime_error&) {}
//     
//     EXPECT_EQ(o1.mStats.CtorCounter, 1);
//     EXPECT_EQ(o1.mStats.CopyCounter, 1);
//     EXPECT_EQ(o1.mStats.DtorCounter, 0);
// 
//     EXPECT_EQ(o2.mStats.CtorCounter, 1);        
//     EXPECT_EQ(o2.mStats.CopyCounter, 1);
//     EXPECT_EQ(o2.mStats.DtorCounter, 0);    
// 
//     EXPECT_EQ(vec.size(), 2);
//     EXPECT_EQ(vec.capacity(), 2);
//     EXPECT_EQ(vec[0].mMagicValue, 1);
//     EXPECT_EQ(vec[1].mMagicValue, 2);
//     EXPECT_EQ(old_data, vec.data());
// }

TEST(VectorX, equal_operator)
{
    vectorx::vector<int> a{ 1, 2, 3 };
    vectorx::vector<int> b{ 1, 2, 3 };
    vectorx::vector<int> c{ 3, 2, 1 };

    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != c);
} 

TEST(VectorX, EmplaceBackNoReallocThrow) 
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;
    T o1{ false, 1 };

    vectorx::vector<T::internal_obj_t> vec{};
    
    vec.reserve(2);
    vec.emplace_back(o1.mInternalObject); 

    o1.mCanThrow = true;

    auto old_data{ vec.data() };

    try 
    {
        vec.emplace_back(o1.mInternalObject);
        FAIL() << "exception expected";
    } 
    catch (const std::runtime_error& e) {}

    EXPECT_EQ(o1.mStats.CtorCounter, 1);
    EXPECT_EQ(o1.mStats.CopyCounter, 1);
    EXPECT_EQ(o1.mStats.MoveCounter, 0);
    EXPECT_EQ(o1.mStats.DtorCounter, 0);

    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.data(), old_data);
    EXPECT_EQ(vec[0].mMagicValue, 1);
}

TEST(VectorX, EmplaceBackReallocThrow) 
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T o1{ false, 1 };
    T o2{ false, 2 };

    vectorx::vector<T::internal_obj_t> vec{};
    
    vec.reserve(1);
    vec.emplace_back(o1.mInternalObject);

    o2.mCanThrow = true;
    auto old_data = vec.data();

    try 
    {
        vec.emplace_back(o2.mInternalObject);
        FAIL() << "exception expected";
    } 
    catch (const std::runtime_error&) {}

    EXPECT_EQ(o1.mStats.CtorCounter, 1);
    EXPECT_EQ(o1.mStats.CopyCounter, 1);
    EXPECT_EQ(o1.mStats.MoveCounter, 0);
    EXPECT_EQ(o1.mStats.DtorCounter, 0);

    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.capacity(), 1);
    EXPECT_EQ(vec.data(), old_data);
    EXPECT_EQ(vec[0].mMagicValue, 1);
}

TEST(VectorX, NoThrowPushBack)
{
    vectorx::vector<int> vec_1{};

    for (std::size_t i{}; i < 1'000; ++i)
    {
        vec_1.push_back(i);
    }

    for (std::size_t i{}; i < 1'000; ++i)
    {
        EXPECT_EQ(vec_1[i], i);
    }

    EXPECT_EQ(std::size(vec_1), 1'000);
    EXPECT_FALSE(std::empty(vec_1));
    EXPECT_NE(std::data(vec_1), nullptr);
}

TEST(VectorX, NoThrowCopyCtor)
{
    vectorx::vector<int> vec_1{};
    for (std::size_t i{}; i < 1'000; ++i)
    {
        vec_1.push_back(i);
    }

    auto vec_2{ vec_1 };

    for (std::size_t i{}; i < 1'000; ++i)
    {
        EXPECT_EQ(vec_1[i], vec_2[i]);
        EXPECT_NE(&vec_1[i], &vec_2[i]);
    }

    EXPECT_EQ(std::size(vec_1), 1'000);
    EXPECT_FALSE(std::empty(vec_1));
    EXPECT_NE(std::data(vec_1), nullptr);

    EXPECT_EQ(std::size(vec_2), 1'000);
    EXPECT_FALSE(std::empty(vec_2));
    EXPECT_NE(std::data(vec_2), nullptr);
}

TEST(VectorX, NoThrowCopyAssignment)
{
    vectorx::vector<int> vec_1{};
    for (std::size_t i{}; i < 1'000; ++i)
    {
        vec_1.push_back(i);
    }

    vectorx::vector<int> vec_2{};
    for (std::size_t i{}; i < 1'000; ++i)
    {
        vec_2.push_back(i * 2);
    }

    vec_1 = vec_2;

    for (std::size_t i{}; i < 1'000; ++i)
    {
        EXPECT_EQ(vec_1[i], vec_2[i]);
        EXPECT_NE(&vec_1[i], &vec_2[i]);
    }

    EXPECT_EQ(std::size(vec_1), 1'000);
    EXPECT_FALSE(std::empty(vec_1));
    EXPECT_NE(std::data(vec_1), nullptr);

    EXPECT_EQ(std::size(vec_2), 1'000);
    EXPECT_FALSE(std::empty(vec_2));
    EXPECT_NE(std::data(vec_2), nullptr);
}

TEST(VectorX, NoThrowReserve)
{
    vectorx::vector<int> vec{ 0, 1, 2, 3, 4, 5, 6 };
    vec.reserve(128);

    for (std::size_t i{}; i < 7; ++i)
    {
        ASSERT_EQ(vec[i], i);
    }

    ASSERT_EQ(std::size(vec), 7);
    ASSERT_EQ(vec.capacity(), 128);
}

TEST(VectorX, NothrowObjectWithAllocsPushBack)
{
    vectorx::vector<NothrowObjectWithAllocs> vec{};
    constexpr std::size_t count{ 100 };

    for (std::size_t i{}; i < count; ++i) 
    {
        vec.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i)));
        
        EXPECT_EQ(vec.size(), i + 1);
        EXPECT_NE(vec.data(), nullptr);
    }
}

TEST(VectorX, NothrowObjectWithAllocsCopyCtor)
{
    vectorx::vector<NothrowObjectWithAllocs> vec_1{};
    for (std::size_t i{}; i < 50; ++i)
    {
        vec_1.push_back(NothrowObjectWithAllocs{ static_cast<std::int32_t>(i) });  
    } 

    auto vec_2{ vec_1 };

    EXPECT_EQ(std::size(vec_1), std::size(vec_2));
    EXPECT_NE(std::data(vec_1), std::data(vec_2));
}

TEST(VectorX, NothrowObjectWithAllocsCopyAssignment)
{
    vectorx::vector<NothrowObjectWithAllocs> vec_1{};
    vectorx::vector<NothrowObjectWithAllocs> vec_2{};
    for (std::size_t i{}; i < 30; ++i) 
    {
        vec_1.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i)));
        vec_2.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i + 100u)));
    }
    
    vec_1 = vec_2;
    
    EXPECT_EQ(std::size(vec_1), std::size(vec_2));
    EXPECT_NE(std::data(vec_1), std::data(vec_2));
}

TEST(VectorX, NothrowObjectWithAllocsReserve)
{
    vectorx::vector<NothrowObjectWithAllocs> vec{};
    for (std::size_t i{}; i < 20; ++i)
    {
        vec.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i)));
    } 
    
    vec.reserve(200);
    
    EXPECT_EQ(std::size(vec), 20);
    EXPECT_GE(vec.capacity(), 200u);
    EXPECT_NE(std::data(vec), nullptr);
}

TEST(VectorX, NothrowObjectWithAllocsMoveCtor)
{
    vectorx::vector<NothrowObjectWithAllocs> vec{};
    for (std::size_t i{}; i < 512; ++i)
    {
        vec.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i)));
    } 

    auto new_vec{ std::move(vec) };
    for (std::size_t i{}; i < std::size(new_vec); ++i)
    {
        EXPECT_EQ(new_vec[i].value(), static_cast<std::int32_t>(i));
    }

    EXPECT_EQ(std::data(vec), nullptr);
    EXPECT_EQ(std::size(vec), 0);
    EXPECT_EQ(vec.capacity(), 0);

    EXPECT_NE(std::data(new_vec), nullptr);
    EXPECT_EQ(std::size(new_vec), 512);
}

TEST(VectorX, NothrowObjectWithAllocsMoveAssignment)
{
    vectorx::vector<NothrowObjectWithAllocs> vec_1{};
    vectorx::vector<NothrowObjectWithAllocs> vec_2{};
    
    for (std::size_t i{}; i < 512; ++i)
    {
        vec_1.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i)));
        vec_2.push_back(NothrowObjectWithAllocs(static_cast<std::int32_t>(i * 2)));
    } 

    vec_1 = std::move(vec_2);

    for (std::size_t i{}; i < std::size(vec_1); ++i)
    {
        EXPECT_EQ(vec_1[i].value(), static_cast<std::int32_t>(i * 2));
    }

    EXPECT_EQ(std::data(vec_2), nullptr);
    EXPECT_EQ(std::size(vec_2), 0);
    EXPECT_EQ(vec_2.capacity(), 0);

    EXPECT_NE(std::data(vec_1), nullptr);
    EXPECT_EQ(std::size(vec_1), 512);
}

TEST(VectorX, ResizeNoThrow)
{
    vectorx::vector<NothrowObjectWithAllocs> vec{};
    vec.resize(25, NothrowObjectWithAllocs{ 22 });

    for (std::size_t i{}; i < std::size(vec); ++i)
    {
        EXPECT_EQ(vec[i].value(), 22);
    }

    EXPECT_EQ(std::size(vec), 25);

    vec.resize(12, NothrowObjectWithAllocs{ 11 });

    for (std::size_t i{}; i < std::size(vec); ++i)
    {
        EXPECT_EQ(vec[i].value(), 22);
    }

    EXPECT_EQ(std::size(vec), 12);
}

TEST(VectorX, ResizeThrowOnCopy)
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T o1{ false, 1 };
    T o2{ false, 2 };
    T o3{ false, 3 };

    vectorx::vector<T::internal_obj_t> vec{ o1.mInternalObject, o2.mInternalObject };
    o3.mCanThrow = true;

    try
    {
        vec.resize(12, o3.mInternalObject);
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e) {}

    EXPECT_EQ(o1.mStats.CtorCounter, 1);
    EXPECT_EQ(o1.mStats.CopyCounter, 2);
    EXPECT_EQ(o1.mStats.MoveCounter, 0);
    EXPECT_EQ(o1.mStats.DtorCounter, 1);

    EXPECT_EQ(o2.mStats.CtorCounter, 1);
    EXPECT_EQ(o2.mStats.CopyCounter, 2);
    EXPECT_EQ(o2.mStats.MoveCounter, 0);
    EXPECT_EQ(o2.mStats.DtorCounter, 1);

    EXPECT_EQ(std::size(vec), 2);
    
    EXPECT_EQ(vec[0].mMagicValue, 1);
    EXPECT_EQ(vec[1].mMagicValue, 2);
}

TEST(VectorX, ComparisonWithStdVector)
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T a1{ false, 1 };
    T a2{ false, 2 };
    T a3{ false, 3 };
    T a4{ false, 4 };

    T b1{ false, 1 };
    T b2{ false, 2 };
    T b3{ false, 3 };
    T b4{ false, 4 };

    vectorx::vector<T::internal_obj_t> vec{};
    std::vector<T::internal_obj_t> std_vec{};

    vec.push_back(a1.mInternalObject);
    std_vec.push_back(b1.mInternalObject);

    EXPECT_EQ(vec[0].mStats, std_vec[0].mStats);
    EXPECT_EQ(std::size(vec), std::size(std_vec));

    vec.push_back(a2.mInternalObject);
    std_vec.push_back(b2.mInternalObject); 

    EXPECT_EQ(vec[0].mStats, std_vec[0].mStats);
    EXPECT_EQ(vec[1].mStats, std_vec[1].mStats);

    EXPECT_EQ(std::size(vec), std::size(std_vec));

    vec.push_back(a3.mInternalObject);
    std_vec.push_back(b3.mInternalObject); 

    EXPECT_EQ(vec[0].mStats, std_vec[0].mStats);
    EXPECT_EQ(vec[1].mStats, std_vec[1].mStats);
    EXPECT_EQ(vec[2].mStats, std_vec[2].mStats);
    
    EXPECT_EQ(std::size(vec), std::size(std_vec));

    vec.push_back(a4.mInternalObject);
    std_vec.push_back(b4.mInternalObject); 

    EXPECT_EQ(vec[0].mStats, std_vec[0].mStats);
    EXPECT_EQ(vec[1].mStats, std_vec[1].mStats);
    EXPECT_EQ(vec[2].mStats, std_vec[2].mStats);
    EXPECT_EQ(vec[3].mStats, std_vec[3].mStats);

    EXPECT_EQ(vec[0].mMagicValue, std_vec[0].mMagicValue);
    EXPECT_EQ(vec[1].mMagicValue, std_vec[1].mMagicValue);
    EXPECT_EQ(vec[2].mMagicValue, std_vec[2].mMagicValue);
    EXPECT_EQ(vec[3].mMagicValue, std_vec[3].mMagicValue);

    EXPECT_EQ(std::size(vec), std::size(std_vec));
}

TEST(VectorX, ComparisonWithStdVectorReserve)
{
    using T = ThrowObject<ThrowPolicy::ThrowOnCopy>;

    T a1{ false, 1 };
    T b1{ false, 1 };

    vectorx::vector<T::internal_obj_t> vec{};
    std::vector<T::internal_obj_t> std_vec{};

    vec.push_back(a1.mInternalObject);
    std_vec.push_back(b1.mInternalObject);

    vec.reserve(128);
    std_vec.reserve(128);

    EXPECT_EQ(vec[0].mStats, std_vec[0].mStats);
}
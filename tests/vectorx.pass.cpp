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
    ObjectStats stats_1{};
    ObjectStats stats_2{};
    ObjectStats stats_3{};
    ObjectStats stats_4{};

    bool can_throw_1{ false };
    bool can_throw_2{ false };
    bool can_throw_3{ false };
    bool can_throw_4{ true };

    try
    {
        vectorx::vector<ThrowObject> vec
        { 
            { 
                ThrowObject{ stats_1, ThrowPolicy::NoThrow, can_throw_1 },
                ThrowObject{ stats_2, ThrowPolicy::NoThrow, can_throw_2},
                ThrowObject{ stats_3, ThrowPolicy::NoThrow, can_throw_3 },
                ThrowObject{ stats_4, ThrowPolicy::ThrowOnCopy, can_throw_4 }, 
            } 
        };

        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e)
    {
        EXPECT_EQ(stats_1.CtorCounter, 1);
        EXPECT_EQ(stats_1.CopyCounter, 1);
        EXPECT_EQ(stats_1.DtorCounter, 2);

        EXPECT_EQ(stats_2.CtorCounter, 1);
        EXPECT_EQ(stats_2.CopyCounter, 1);
        EXPECT_EQ(stats_2.DtorCounter, 2);

        EXPECT_EQ(stats_3.CtorCounter, 1);
        EXPECT_EQ(stats_3.CopyCounter, 1);
        EXPECT_EQ(stats_3.DtorCounter, 2);

        EXPECT_EQ(stats_4.CtorCounter, 1);
        EXPECT_EQ(stats_4.CopyCounter, 0);
        EXPECT_EQ(stats_4.DtorCounter, 1);
    }
}

TEST(VectorX, CopyCtorThrow)
{
    ObjectStats statsa_1{};
    ObjectStats statsa_2{};
    ObjectStats statsa_3{};
    ObjectStats statsa_4{};

    ObjectStats statsb_1{};
    ObjectStats statsb_2{};
    ObjectStats statsb_3{};
    ObjectStats statsb_4{};

    bool can_throwa_1{ false };
    bool can_throwa_2{ false };
    bool can_throwa_3{ false };
    bool can_throwa_4{ false };

    bool can_throwb_1{ false };
    bool can_throwb_2{ false };
    bool can_throwb_3{ false };
    bool can_throwb_4{ false };

    ThrowObject obja_1{ statsa_1, ThrowPolicy::NoThrow, can_throwa_1, 1 };
    ThrowObject obja_2{ statsa_2, ThrowPolicy::NoThrow, can_throwa_2, 2 };
    ThrowObject obja_3{ statsa_3, ThrowPolicy::NoThrow, can_throwa_3, 3 };
    ThrowObject obja_4{ statsa_4, ThrowPolicy::ThrowOnCopy, can_throwa_4, 4 };

    ThrowObject objb_1{ statsb_1, ThrowPolicy::NoThrow, can_throwb_1, 11 };
    ThrowObject objb_2{ statsb_2, ThrowPolicy::NoThrow, can_throwb_2, 22 };
    ThrowObject objb_3{ statsb_3, ThrowPolicy::NoThrow, can_throwb_3, 33 };
    ThrowObject objb_4{ statsb_4, ThrowPolicy::NoThrow, can_throwb_4, 44 };

    vectorx::vector<ThrowObject> veca{ obja_1, obja_2, obja_3, obja_4 };
    vectorx::vector<ThrowObject> vecb{ objb_1, objb_2, objb_3, objb_4 };

    try
    {
        can_throwa_4 = true;
        vecb = veca;
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error& e)
    {
        // A
        EXPECT_EQ(statsa_1.CtorCounter, 1);
        EXPECT_EQ(statsa_1.CopyCounter, 3);
        EXPECT_EQ(statsa_1.DtorCounter, 2);

        EXPECT_EQ(statsa_2.CtorCounter, 1);
        EXPECT_EQ(statsa_2.CopyCounter, 3);
        EXPECT_EQ(statsa_2.DtorCounter, 2);

        EXPECT_EQ(statsa_3.CtorCounter, 1);
        EXPECT_EQ(statsa_3.CopyCounter, 3);
        EXPECT_EQ(statsa_3.DtorCounter, 2);

        EXPECT_EQ(statsa_4.CtorCounter, 1);
        EXPECT_EQ(statsa_4.CopyCounter, 2);
        EXPECT_EQ(statsa_4.DtorCounter, 1);

        // B
        EXPECT_EQ(statsb_1.CtorCounter, 1);
        EXPECT_EQ(statsb_1.CopyCounter, 2);
        EXPECT_EQ(statsb_1.DtorCounter, 1);

        EXPECT_EQ(statsb_2.CtorCounter, 1);
        EXPECT_EQ(statsb_2.CopyCounter, 2);
        EXPECT_EQ(statsb_2.DtorCounter, 1);

        EXPECT_EQ(statsb_3.CtorCounter, 1);
        EXPECT_EQ(statsb_3.CopyCounter, 2);
        EXPECT_EQ(statsb_3.DtorCounter, 1);

        EXPECT_EQ(statsb_4.CtorCounter, 1);
        EXPECT_EQ(statsb_4.CopyCounter, 2);
        EXPECT_EQ(statsb_4.DtorCounter, 1);
    }

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
    ObjectStats stats1{}; 
    ObjectStats stats2{};
    
    bool can_throw1{ false }; 
    bool can_throw2{ false };
    
    {
        ThrowObject obj1{ stats1, ThrowPolicy::NoThrow, can_throw1, 100 };
        ThrowObject obj2{ stats2, ThrowPolicy::NoThrow, can_throw2, 200 };
    
        vectorx::vector<ThrowObject> vec{};

        vec.push_back(obj1);
        vec.push_back(obj2);

        ObjectStats stats3{};
        bool can_throw3{ true };
        ThrowObject obj3{ stats3, ThrowPolicy::ThrowOnCopy, can_throw3, 300 };

        try 
        {
            vec.push_back(obj3);
            FAIL() << "exception expected";
        }
        catch (const std::runtime_error&) 
        {
            EXPECT_EQ(stats1.CtorCounter, 1);
            EXPECT_EQ(stats1.CopyCounter, 2);
            EXPECT_EQ(stats1.DtorCounter, 1);

            EXPECT_EQ(stats2.CtorCounter, 1);
            EXPECT_EQ(stats2.CopyCounter, 2);
            EXPECT_EQ(stats2.DtorCounter, 1);

            EXPECT_EQ(stats3.CtorCounter, 1);
            EXPECT_EQ(stats3.CopyCounter, 0);
            EXPECT_EQ(stats3.DtorCounter, 0); // was not constructed via push_back
        }

        EXPECT_EQ(vec.size(), 2);
        EXPECT_EQ(vec.capacity(), 2);
        EXPECT_EQ(vec[0].mMagicValue, 100);
        EXPECT_EQ(vec[1].mMagicValue, 200);
    }

    EXPECT_EQ(stats1.DtorCounter, 3);
    EXPECT_EQ(stats2.DtorCounter, 3);
}

TEST(VectorX, ReserveCopyThrow) 
{
    ObjectStats stats_1{}; 
    ObjectStats stats_2{}; 

    bool can_throw1{ false };
    bool can_throw2{ false};

    ThrowObject obj1{ stats_1, ThrowPolicy::NoThrow, can_throw1, 10 };
    ThrowObject obj2{ stats_2, ThrowPolicy::ThrowOnCopy, can_throw2, 20 };

    vectorx::vector<ThrowObject> vec;

    vec.reserve(2);
    vec.push_back(obj1);
    vec.push_back(obj2);

    can_throw2 = true;

    auto* old_data{ vec.data() };

    try 
    {
        vec.reserve(128);
        FAIL() << "exception expected";
    }
    catch (const std::runtime_error&) 
    {
        EXPECT_EQ(stats_1.CtorCounter, 1);
        EXPECT_EQ(stats_1.CopyCounter, 2);
        EXPECT_EQ(stats_1.DtorCounter, 1);

        EXPECT_EQ(stats_2.CtorCounter, 1);        
        EXPECT_EQ(stats_2.CopyCounter, 1);
        EXPECT_EQ(stats_2.DtorCounter, 0);    
    }

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 2);
    EXPECT_EQ(vec[0].mMagicValue, 10);
    EXPECT_EQ(vec[1].mMagicValue, 20);
    EXPECT_EQ(old_data, vec.data());
}

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
    ObjectStats stats_1{}; 
    bool can_throw_1{ false };

    ThrowObject obj_1{ stats_1, ThrowPolicy::ThrowOnCopy, can_throw_1, 100 };

    vectorx::vector<ThrowObject> vec{};
    vec.reserve(2);
    vec.emplace_back(obj_1); 

    can_throw_1 = true;

    auto old_data{ vec.data() };

    try 
    {
        vec.emplace_back(obj_1);
        FAIL() << "exception expected";
    } 
    catch (const std::runtime_error& e) 
    {
        EXPECT_EQ(stats_1.CtorCounter, 1);
        EXPECT_EQ(stats_1.CopyCounter, 1);
        EXPECT_EQ(stats_1.DtorCounter, 0);
    }

    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.data(), old_data);
    EXPECT_EQ(vec[0].mMagicValue, 100);
}

TEST(VectorX, EmplaceBackReallocThrow) 
{
    ObjectStats stats_1{}; 
    ObjectStats stats_2{}; 

    bool can_throw_1{ false };
    bool can_throw_2{ false };

    ThrowObject obj1{ stats_1, ThrowPolicy::NoThrow,     can_throw_1, 10 };
    ThrowObject obj2{ stats_2, ThrowPolicy::ThrowOnCopy, can_throw_2, 20 };

    vectorx::vector<ThrowObject> vec;
    
    vec.reserve(1);
    vec.emplace_back(obj1);

    can_throw_2 = true;
    auto old_data = vec.data();

    try 
    {
        vec.emplace_back(obj2);
        FAIL() << "exception expected";
    } 
    catch (const std::runtime_error&) 
    { 
        EXPECT_EQ(stats_1.CtorCounter, 1);
        EXPECT_EQ(stats_1.CopyCounter, 2);
        EXPECT_EQ(stats_1.DtorCounter, 1);
    }

    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.capacity(), 1);
    EXPECT_EQ(vec.data(), old_data);
    EXPECT_EQ(vec[0].mMagicValue, 10);
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

// maybe rewrite
TEST(VectorX, ComparisonWithStdVector)
{
    NothrowObject obj_a{};
    NothrowObject obj_b{};
    NothrowObject obj_c{};

    NothrowObject std_obj_a{};
    NothrowObject std_obj_b{};
    NothrowObject std_obj_c{};

    vectorx::vector<NothrowObject> vec{};
    std::vector<NothrowObject> std_vec{};

    vec.push_back(obj_a);
    std_vec.push_back(std_obj_a);

    EXPECT_EQ(vec[0], std_vec[0]);
    EXPECT_EQ(std::size(vec), std::size(std_vec));

    vec.push_back(obj_b);
    std_vec.push_back(std_obj_b); 

    EXPECT_EQ(vec[1], std_vec[1]);
    EXPECT_EQ(std::size(vec), std::size(std_vec));

    vec.push_back(obj_c);
    std_vec.push_back(std_obj_c); 

    EXPECT_EQ(vec[2], std_vec[2]);
    EXPECT_EQ(std::size(vec), std::size(std_vec));
}
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

TEST(IteratorRAI, RangeBased)
{
    vectorx::vector<int> vec{ 1, 2, 3, 4, 5, 6 };
    int i{};
    
    for (auto el : vec)
    {
        EXPECT_EQ(el, ++i);
    }
}

TEST(IteratorRAI, DereferenceAndArrow)
{
    vectorx::vector<int> vec{10, 20, 30};
    auto it{ vec.begin() };
    
    EXPECT_EQ(*it, 10);
    EXPECT_EQ(*(it + 2), 30);
    EXPECT_EQ(*it, *(it.operator->()));
}

TEST(IteratorRAI, IncrementOperators)
{
    vectorx::vector<int> vec{1, 2, 3};
    
    auto it{ vec.begin() };
    auto pre{++it};
    
    EXPECT_EQ(*pre, 2);
    EXPECT_EQ(*it, 2);

    auto post{ it++ };

    EXPECT_EQ(*post, 2);
    EXPECT_EQ(*it, 3);
}

TEST(IteratorRAI, DecrementOperators)
{
    vectorx::vector<int> vec{1, 2, 3};
    
    auto it{ vec.end() - 1 };
    auto pre{ --it };
    
    EXPECT_EQ(*pre, 2);
    EXPECT_EQ(*it, 2);

    auto post{ it-- };
    
    EXPECT_EQ(*post, 2);
    EXPECT_EQ(*it, 1);
}

TEST(IteratorRAI, ArithmeticOperators)
{
    vectorx::vector<int> vec{0, 10, 20, 30, 40};
    
    auto it{ vec.begin() };
    it += 3;
    EXPECT_EQ(*it, 30);

    it -= 2;
    EXPECT_EQ(*it, 10);

    auto it2{ it + 2 };
    EXPECT_EQ(*it2, 30);

    auto it3{ 2 + it };
    EXPECT_EQ(*it3, 30);

    auto dist{ it3 - it };
    EXPECT_EQ(dist, 2);
}

TEST(IteratorRAI, SubscriptOperator)
{
    vectorx::vector<int> vec{5, 6, 7, 8};
    
    auto it{ vec.begin() };
    EXPECT_EQ(it[0], 5);
    EXPECT_EQ(it[3], 8);
}

TEST(IteratorRAI, ComparisonOperators)
{
    vectorx::vector<int> vec{1, 2, 3, 4};
    
    auto a{ vec.begin() };
    auto b{ a + 2 };
    auto c{ vec.begin() + 2 };

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(b == c);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(IteratorRAI, StdDistance)
{
    vectorx::vector<int> vec{0, 1, 2, 3, 4, 5};
    auto dist{ std::distance(vec.begin(), vec.begin() + 5) };
    
    EXPECT_EQ(dist, 5);
}

template <typename It>
void testConstConversion(It) {}

TEST(IteratorRAI, ConstIteratorConversion)
{
    vectorx::vector<int> vec{1, 2, 3};
    
    auto it{ vec.begin() };
    testConstConversion<vectorx::vector<int>::const_iterator>(it);
}
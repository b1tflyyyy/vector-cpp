// MIT License

// Copyright (c) 2024 Andrii

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <cstdio>
#include <gtest/gtest.h>
#include <custom_vector.hpp>
#include <string>
#include <format>

// ----------------------------------------------------------------------------------------------------------------------
TEST(vector, ctor)
{
    // default ctor
    custom::vector<int> vec_1{};
    ASSERT_EQ(0, vec_1.size());
    ASSERT_EQ(2, vec_1.capacity());
    ASSERT_NE(nullptr, vec_1.data());
    ASSERT_EQ(true, vec_1.is_empty());

    // init with value
    int val = 22;
    std::size_t size{ 5 };
    custom::vector<int> vec_2(size, val);
    ASSERT_EQ(size, vec_2.size());
    ASSERT_EQ(size * 2, vec_2.capacity());
    ASSERT_NE(nullptr, vec_2.data());
    ASSERT_EQ(false, vec_2.is_empty());
    auto  ptr{ std::data(vec_2) };
    std::cout << "explicit vector(size_type count, const_reference value, const Alloc& alloc = Alloc()) ";
    for (std::size_t i{}; i < size; ++i)
    {
        std::cout << ptr[i] << ' ';
        ASSERT_EQ(ptr[i], val);
    }
    std::cout << std::endl;

    // init with init_list
    custom::vector<int> vec_3{ 1, 2, 3, 4, 5 };
    ASSERT_EQ(5, vec_3.size());
    ASSERT_EQ(10, vec_3.capacity());
    ASSERT_NE(nullptr, vec_3.data());
    ASSERT_EQ(false, vec_3.is_empty());
    auto  ptr_1{ std::data(vec_3) };
    std::cout << "constexpr vector(std::initializer_list<value_type> list, const Alloc& alloc = Alloc()) ";
    for (std::size_t i{}; i < 5; ++i)
    {
        std::cout << ptr_1[i] << ' ';
        ASSERT_EQ(ptr_1[i], i + 1);
    }
    std::cout << std::endl;
}

// ----------------------------------------------------------------------------------------------------------------------
TEST(vector, copy_ctor)
{
    // copy ctor
    custom::vector<int> vec_1{ 1, 2, 3, 4, 5, 6, 7, 8 };
    custom::vector<int> vec_2{ vec_1 };

    ASSERT_NE(std::data(vec_1), std::data(vec_2));
    ASSERT_EQ(vec_1.size(), vec_2.size());
    ASSERT_EQ(vec_1.capacity(), vec_2.capacity());
    auto ptr_1{ std::data(vec_1) };
    auto ptr_2{ std::data(vec_2) };
    std::cout << "Copy ctor: ";
    for (std::size_t i{}; i < vec_1.size(); ++i)
    {
        std::cout << std::format("pair {}:{} | ", ptr_1[i], ptr_2[i]);
        ASSERT_EQ(ptr_1[i], ptr_2[i]);
    }
    std::cout << std::endl;

    // copy assignment operator
    // case 1
    custom::vector<int> vec_3{ 1, 2, 3, 4, 5, 6 };
    custom::vector<int> vec_4{ 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    
    vec_4 = vec_3;
    ASSERT_EQ(vec_4.size(), vec_3.size());
    auto ptr_4{ std::data(vec_4) };
    auto ptr_3{ std::data(vec_3) };
    std::cout << "Copy Assignment ctor case 1: ";
    for (std::size_t i{}; i < vec_4.size(); ++i)
    {
        std::cout << std::format("pair {}:{} | ", ptr_1[i], ptr_2[i]);
        ASSERT_EQ(ptr_4[i], ptr_3[i]);
    }
    std::cout << std::endl;

    // case 2
    custom::vector<int> vec_5{ 1, 2, 3, 4, 5, 6 };
    custom::vector<int> vec_6{ 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    
    vec_5 = vec_6;
    ASSERT_EQ(vec_5.size(), vec_6.size());
    std::cout << "Copy Assignment ctor case 2: ";
    for (std::size_t i{}; i < vec_5.size(); ++i)
    {
        std::cout << std::format("pair {}:{} | ", vec_5[i], vec_6[i]);
        ASSERT_EQ(vec_6[i], vec_5[i]);
    }
    std::cout << std::endl;

    // case 3
    custom::vector<int> vec_7{ 1, 2, 3, 4, 5 };
    vec_7 = vec_7;

    // case 4
    custom::vector<std::string> v1{ "hello", "hello1", "hello2", "hello3", "hello4", "hello5" };
    custom::vector<std::string> v2{ v1 };
    
    auto ptrr_1{ std::data(v1) };
    auto ptrr_2{ std::data(v2) };

    for (std::size_t i{}; i < v1.size(); ++i)
    {
        ASSERT_EQ(ptrr_1[i], ptrr_2[i]);
        ASSERT_NE(&ptrr_1[i], &ptrr_2[i]);
    }

    // case 5
    custom::vector<int> v3{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    custom::vector<int> v4{};
    for (std::size_t i{}; i < 500; ++i)
    {
        v4.push_back(i);
    }
    for (std::size_t i{}; i < 500; ++i)
    {
        ASSERT_EQ(v4[i], i);
    }

    v4 = v3;

    auto p3{ std::data(v3) };
    auto p4{ std::data(v4) };
    for (std::size_t i{}; i < v3.size() || i < v4.size(); ++i)
    {
        ASSERT_EQ(p3[i], p4[i]);
        ASSERT_NE(&p3[i], &p4[i]);
    }

    v3.push_back(10);
    v3.push_back(11);

    for (std::size_t i{}; i < 11; ++i)
    {
        ASSERT_EQ(v3[i], i + 1);
    }
}

// ----------------------------------------------------------------------------------------------------------------------
TEST(vector, move_ctor)
{
    // move ctor
    custom::vector<int> a{ 1, 2, 3, 4, 5, 6, 7, 8 };
    auto a_sz{ a.size() };
    auto a_cp{ a.capacity() };
    auto a_ptr{ std::data(a) };

    auto b{ std::move(a) };

    ASSERT_EQ(b.size(), a_sz);
    ASSERT_EQ(b.capacity(), a_cp);
    ASSERT_EQ(std::data(b), a_ptr);
    ASSERT_EQ(0, a.size());
    ASSERT_EQ(2, a.capacity());
    ASSERT_NE(std::data(a), std::data(b));

    // move assignment ctor
    custom::vector<int> v1{ 1, 2, 3, 4, 5, 6 };
    auto v_sz{ v1.size() };
    auto v_cp{ v1.capacity() };
    auto v_ptr{ std::data(v1) };

    custom::vector<int> v2{ 3, 4, 5, 6, 7, 8 };
    v2 = std::move(v1);

    ASSERT_EQ(v2.size(), v_sz);
    ASSERT_EQ(v2.capacity(), v_cp);
    ASSERT_EQ(std::data(v2), v_ptr);
    ASSERT_EQ(0, v1.size());
    ASSERT_EQ(2, v1.capacity());
    ASSERT_NE(std::data(v1), std::data(v2));

    custom::vector<int> v3{ 1, 2, 3, 4, 5, 6, 7 };
    custom::vector<int> v4{ 9, 8, 5 };

    v3 = std::move(v4);

    ASSERT_EQ(v3[0], 9);
    ASSERT_EQ(v3[1], 8);
    ASSERT_EQ(v3[2], 5);
    ASSERT_EQ(v3.size(), 3);

    for (std::size_t i{}; i < 1'000; ++i)
    {
        v4.push_back(i);
    }
    for (std::size_t i{}; i < 1'000; ++i)
    {
        ASSERT_EQ(v4[i], i);
    }
}

// ----------------------------------------------------------------------------------------------------------------------
TEST(vector, iterator)
{
    custom::vector<int> vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::fill(std::begin(vec), std::end(vec), 66);

    for (const auto el : vec)
    {
        ASSERT_EQ(el, 66);
    }
}
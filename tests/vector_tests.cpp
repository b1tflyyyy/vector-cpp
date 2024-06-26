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

#include <gtest/gtest.h>
#include <custom_vector.hpp>
#include <format>

// ----------------------------------------------------------------------------------------------------------------------
TEST(vector, ctor)
{
    // default ctor
    custom::vector<int> vec_1{};
    ASSERT_EQ(0, vec_1.size());
    ASSERT_EQ(0, vec_1.capacity());
    ASSERT_EQ(nullptr, vec_1.data());
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
}
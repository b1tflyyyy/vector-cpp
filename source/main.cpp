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


#include <cassert>
#include <cstddef>
#include <iostream>
#include <cstdint>
#include <custom_vector.hpp>
#include <string>
#include <vector>

static std::ptrdiff_t counter{};

struct A
{
    A()
    {
        ++counter;
        std::cout << "A() ctor\n";
    }
    ~A()
    {
        --counter;
        std::cout << "~A() dtor\n";
    }
    A(const A& other)
    {
        ++counter;
        std::cout << "A() copy ctor\n";
    }
    A& operator=(const A&)
    {
        ++counter;
        std::cout << "A=() ctor\n";
        return *this;
    }
    A(A&&) noexcept
    {
        ++counter;
        std::cout << "A move ctor\n";
    }

    std::uint64_t* Ptr;
};

int main()
{
    std::vector<int> a{};
    #if 0
    {
    custom::vector<A> vec{};
    A a{};
    for (std::size_t i{}; i < 3253; ++i)
    {
        vec.push_back(a);
    }
    }
    std::cout << "Counter: " << counter << '\n';
    assert(counter == 0);
    std::cout << std::endl;
    #endif
    
    {
    custom::vector<std::string> vec{ "hello", "hello1", "hello2", "hello3", "hello4", "hello5" };
    for (const auto& el : vec)
    {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
    }

    return 0;
}
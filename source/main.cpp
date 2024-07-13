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
#include <cstdio>
#include <iostream>
#include <cstdint>
#include <custom_vector.hpp>
#include <string>
#include <vector>
#include <format>

static std::ptrdiff_t counter{};

#if 0
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
#endif

template <typename C>
void Show_Cont(C& cont)
{
    for (const auto& el : cont)
    {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}


int main()
{
    custom::vector<std::string> vec{ "hello", "world", "new" };
    Show_Cont(vec);
    
    auto v1{ std::move(vec) };
    Show_Cont(v1);
    
    std::cout << std::format("vec_cap: {} - vec_size: {}\n", vec.capacity(), vec.size());
    vec.push_back("hello world how are you");
    Show_Cont(vec);
    std::cout << std::format("vec cap: {} - vec size: {}\n", vec.capacity(), vec.size());



    return 0;
}

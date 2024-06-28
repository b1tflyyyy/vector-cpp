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
#include <cstddef>

// ----------------------------------------------------------------------------------------------------------------------
#define ENABLE_MEMORY_LOGS_CUSTOM_VECTOR false

// ----------------------------------------------------------------------------------------------------------------------
static std::ptrdiff_t Allocated_Objects_Count{};
static std::ptrdiff_t Deallocated_Objects_Count{};

// ----------------------------------------------------------------------------------------------------------------------
struct SMemory_Logger
{
    SMemory_Logger(std::size_t a = 25) : 
        A{ a }
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "() ctor\n";
        #endif

        ++Allocated_Objects_Count;
    }

    SMemory_Logger(const SMemory_Logger& other) :
        A{ other.A }
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "(const T&) copy ctor\n";
        #endif

        ++Allocated_Objects_Count;
    }

    SMemory_Logger& operator=(const SMemory_Logger& other)
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "operator=(const T&)\n";
        #endif

        A = other.A;
        ++Allocated_Objects_Count;
        return *this;
    }

    SMemory_Logger(SMemory_Logger&& other) noexcept : 
        A{ other.A }
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "(T&&) move ctor\n";
        #endif

        other.A = 0;
        ++Allocated_Objects_Count;
    }

    SMemory_Logger& operator=(SMemory_Logger&& other) noexcept
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "operator=(T&&)\n";
        #endif

        A = other.A;
        other.A = 0;
        ++Allocated_Objects_Count;
        return *this;
    }

    ~SMemory_Logger()
    {
        #if ENABLE_MEMORY_LOGS_CUSTOM_VECTOR
        std::cout << "~() dtor\n";
        #endif
        ++Deallocated_Objects_Count;
    }
    
    std::size_t A; 
};


TEST(memory_leaks, case_1)
{
    {
    custom::vector<SMemory_Logger> vec{};
    for (std::size_t i{}; i < 7; ++i)
    {
        vec.push_back(SMemory_Logger{ i });
    }

    vec.reserve(66);
    vec.resize(12);

    for (std::size_t i{}; i < 26; ++i)
    {
        vec.emplace_back(66);
    }

    vec.resize(5);
    }

    std::cout << std::format("Allocated objects: {}, Deallocated Objects: {}\n", Allocated_Objects_Count, Deallocated_Objects_Count);
    ASSERT_EQ(Allocated_Objects_Count - Deallocated_Objects_Count, 0);
}
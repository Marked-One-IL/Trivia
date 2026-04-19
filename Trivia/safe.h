#pragma once
#include <syncstream>
#include <print>
#include <iostream>

// Thread safe.
namespace safe 
{
    template <typename... Args>
    void print(std::format_string<Args...> fmt, Args&&... args) 
    {
        std::osyncstream sync_out(std::cout);
        std::print(sync_out, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void println(std::format_string<Args...> fmt, Args&&... args)
    {
        std::osyncstream sync_out(std::cout);
        std::println(sync_out, fmt, std::forward<Args>(args)...);
    }
}
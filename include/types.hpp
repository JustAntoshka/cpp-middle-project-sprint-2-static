#pragma once

#include <stddef.h>
#include <algorithm>
#include <tuple>

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины

template <size_t Size>
struct fixed_string {
    constexpr fixed_string(const char (&str)[Size]) {
        std::copy_n(str, Size, data);
    }
    
    template <size_t Size2>
    constexpr fixed_string(const char (&str)[Size2]) {
        static_assert(Size2 < Size, "Given string is too long");
        std::copy_n(str, Size2, data);
        data[Size2] = '\0';
    }
    
    template<typename Ptr>
    constexpr fixed_string(Ptr begin, Ptr end) {
        std::copy(begin, end, data);
        data[end - begin] = '\0';
    }

    constexpr size_t size() const {
        return Size;
    }

    char data[Size];
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
struct parse_error : fixed_string<100> {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
class scan_result {
    std::tuple<Ts...> v;

public:
    constexpr scan_result(std::tuple<Ts...> _v) 
    : v(_v)
    {}

    constexpr const std::tuple<Ts...>& values() {
        return v;
    }
};

} // namespace stdx::details

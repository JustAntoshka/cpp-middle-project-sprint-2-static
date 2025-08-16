#pragma once

#include <stddef.h>
#include <tuple>

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины

template <size_t Size>
struct fixed_string {
    constexpr fixed_string(const char (&str)[Size]) {
        for(size_t i = 0; i < Size; ++i) {
            data[i] = str[i];
        }
    }
    
    template <size_t Size2>
    constexpr fixed_string(const char (&str)[Size2]) {
        static_assert(Size2 < Size, "Given string is too long");
        for(size_t i = 0; i < Size2; ++i) {
            data[i] = str[i];
        }
    }

    template <typename It>
    constexpr fixed_string(It begin, It end);

    constexpr size_t size() const {
        return Size;
    }

    char data[Size] = {};
};

template <size_t Size>
fixed_string(const char (&)[Size]) -> fixed_string<Size>;

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
struct parse_error : fixed_string<100> {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> v;

    std::tuple<Ts...> values() {
        return v;
    }
};

} // namespace stdx::details

#pragma once

#include <stddef.h>

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины

template <size_t Len = 0>
struct fixed_string {
public:
    constexpr fixed_string(const char (&str)[Len + 1]) {
        size_t i = 0;
        for(; i < Len; ++i) {
            _string[i] = str[i];
        }
        _string[i] = '\0';
    }
    
    template <size_t Len2>
    constexpr fixed_string(const char (&str)[Len2]) {
        static_assert(Len2 < Len, "Given string is too long");
        size_t i = 0;
        for(; i < Len2; ++i) {
            _string[i] = str[i];
        }
        _string[i] = '\0';
    }

    template <typename It>
    constexpr fixed_string(It begin, It end);

private:
    char _string[Len + 1] = {};
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга

// ваш код здесь
struct parse_error {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
struct scan_result {
// ваш код здесь
// измените реализацию
    int i;
};

} // namespace stdx::details

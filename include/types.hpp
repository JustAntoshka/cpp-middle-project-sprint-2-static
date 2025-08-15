#pragma once

#include <stddef.h>

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины

template <size_t Len>
struct fixed_string {
public:
    constexpr fixed_string(const char (&str)[Len]) {
        for(size_t i = 0; i < Len; ++i) {
            _string[i] = str[i];
        }
    }
    
    template <size_t Len2>
    constexpr fixed_string(const char (&str)[Len2]) {
        static_assert(Len2 < Len, "Given string is too long");
        for(size_t i = 0; i < Len2; ++i) {
            _string[i] = str[i];
        }
    }

    template <typename It>
    constexpr fixed_string(It begin, It end);

private:
    char _string[Len] = {};
};

template <size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга

template <size_t Len>
struct parse_error : fixed_string<Len> {

    constexpr parse_error(const char (&str)[Len])
    : fixed_string<Len>(str)
    {}
};

template <size_t N>
parse_error(const char(&)[N]) -> parse_error<N>;

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
struct scan_result {
// ваш код здесь
// измените реализацию
    int i;
};

} // namespace stdx::details

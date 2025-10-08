#pragma once

#include <stddef.h>
#include <algorithm>
#include <stdexcept>
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
    
    constexpr fixed_string(const char* begin, const char* end) {
        if(end - begin > Size) {
            throw std::runtime_error("Incorrect pointer order");
        }
        std::copy(begin, end, data);
        data[end - begin] = '\0';
    }

    constexpr size_t size() const {
        return Size;
    }

    char data[Size];
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки парсинга
constexpr size_t PARSE_ERROR_MAX_LEN = 100;
struct parse_error : fixed_string<PARSE_ERROR_MAX_LEN> {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Ts>
class scan_result {
    std::tuple<Ts...> v;

public:
    constexpr scan_result(std::tuple<Ts...> _v) 
    : v(std::move(_v))
    {}

    constexpr const std::tuple<Ts...>& values() {
        return v;
    }
};

} // namespace stdx::details

#pragma once

#include <cstdint>
#include <string_view>
#include <cstring>
#include <type_traits>
#include <concepts>

#include "format_string.hpp"
#include "types.hpp"


namespace utils {
    template<typename T>
    constexpr const char* MAX_NUM_AS_STR;

    template<typename T>
    constexpr const char* MIN_NUM_AS_STR;

    template<> constexpr const char MIN_NUM_AS_STR<int8_t>[] = "-128";
    template<> constexpr const char MAX_NUM_AS_STR<int8_t>[] = "127";

    template<> constexpr const char MIN_NUM_AS_STR<uint8_t>[] = "0";
    template<> constexpr const char MAX_NUM_AS_STR<uint8_t>[] = "255";

    template<> constexpr const char MIN_NUM_AS_STR<int16_t>[] = "-32768";
    template<> constexpr const char MAX_NUM_AS_STR<int16_t>[] = "32767";

    template<> constexpr const char MIN_NUM_AS_STR<uint16_t>[] = "0";
    template<> constexpr const char MAX_NUM_AS_STR<uint16_t>[] = "65535";

    template<> constexpr const char MIN_NUM_AS_STR<int32_t>[] = "-2147483648";
    template<> constexpr const char MAX_NUM_AS_STR<int32_t>[] = "2147483647";

    template<> constexpr const char MIN_NUM_AS_STR<uint32_t>[] = "0";
    template<> constexpr const char MAX_NUM_AS_STR<uint32_t>[] = "4294967295";

    template<> constexpr const char MIN_NUM_AS_STR<int64_t>[] = "-9223372036854775808";
    template<> constexpr const char MAX_NUM_AS_STR<int64_t>[] = "9223372036854775807";

    template<> constexpr const char MIN_NUM_AS_STR<uint64_t>[] = "0";
    template<> constexpr const char MAX_NUM_AS_STR<uint64_t>[] = "18446744073709551615";

    consteval const char* trim_zeros(const char *str) {
        while(*str == '0') str++;
        return *str ? str : str - 1;
    }

    consteval bool is_digit(char c) {
        return '0' <= c && c <= '9';
    }

    consteval bool is_negative(const char* str) {
        return str[0] == '-';
    }

    consteval bool is_positive_number(const char *str) {
        if(!*str) return false;
        while(*str && is_digit(*str)) ++str;
        return !*str;
    }

    consteval bool is_number(const char *str) {
        return is_negative(str) ? is_positive_number(str + 1) : is_positive_number(str);
    }

    consteval bool greater_or_eq_abs(const char *str1, const char *str2) {
        auto trimmed_str1 = trim_zeros(str1);
        auto trimmed_str2 = trim_zeros(str2);

        const size_t LEN_1 = std::strlen(trimmed_str1);
        const size_t LEN_2 = std::strlen(trimmed_str2);

        if(LEN_1 != LEN_2) {
            return LEN_1 > LEN_2;
        }

        while(*trimmed_str1 && *trimmed_str1 == *trimmed_str2) {
            ++trimmed_str1;
            ++trimmed_str2;
        }
        return *trimmed_str1 >= *trimmed_str2;
    }

    template<size_t N1, size_t N2>
    consteval bool greater_or_eq(const char (&str1)[N1], const char (&str2)[N2]) {
        if (is_negative(str1) && is_negative(str2)) {
            return greater_or_eq_abs(str2 + 1, str1 + 1);
        }
        if (!is_negative(str1) && !is_negative(str2)) {
            return greater_or_eq_abs(str1, str2);
        }
        return is_negative(str2);
    }

    template<typename T, size_t N>
    consteval bool is_in_range(const char (&str)[N]) {
        return greater_or_eq(str, MIN_NUM_AS_STR<std::remove_cv_t<T>>) && greater_or_eq(MAX_NUM_AS_STR<std::remove_cv_t<T>>, str);
    }

    template <typename T, stdx::details::fixed_string src>
    consteval T parse_number() {
        constexpr bool is_neg = is_negative(src.data);
        const char* str = is_neg ? src.data + 1 : src.data;
        std::remove_cv_t<T> result = is_neg ? ('0' - *str) : (*str - '0');
        while(*++str) {
            result *= 10;
            result += is_neg ? ('0' - *str) : (*str - '0');
        }
        return result;
    }
} // namespace utils

namespace stdx::details {

// Шаблонная функция, возвращающая пару позиций в строке с исходными данными, соотвествующих I-ому плейсхолдеру
template<int I, format_string fmt, fixed_string source>
consteval auto get_current_source_for_parsing() {
    static_assert(I >= 0 && I < fmt.number_placeholders, "Invalid placeholder index");

    constexpr auto to_sv = [](const auto& fs) {
        return std::string_view(fs.data, fs.size() - 1);
    };

    constexpr auto fmt_sv = to_sv(fmt.fmt);
    constexpr auto src_sv = to_sv(source);
    constexpr auto& positions = fmt.placeholder_positions;

    // Получаем границы текущего плейсхолдера в формате
    constexpr auto pos_i = positions[I];
    constexpr size_t fmt_start = pos_i.first, fmt_end = pos_i.second;

    // Находим начало в исходной строке
    constexpr auto src_start = [&]{
        if constexpr (I == 0) {
            return fmt_start;
        } else {
            // Находим конец предыдущего плейсхолдера в исходной строке
            constexpr auto prev_bounds = get_current_source_for_parsing<I-1, fmt, source>();
            const auto prev_end = prev_bounds.second;

            // Получаем разделитель между текущим и предыдущим плейсхолдерами
            constexpr auto prev_fmt_end = positions[I-1].second;
            constexpr auto sep = fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

            // Ищем разделитель после предыдущего значения
            auto pos = src_sv.find(sep, prev_end);
            return pos != std::string_view::npos ? pos + sep.size() : src_sv.size();
        }
    }();

    // Находим конец в исходной строке
    constexpr auto src_end = [&]{
        // Получаем разделитель после текущего плейсхолдера
        if constexpr(fmt_end == (fmt_sv.size() - 1)) {
            return src_sv.size();
        }
        constexpr auto sep = fmt_sv.substr(fmt_end + 1,
            (I < fmt.number_placeholders - 1)
                ? positions[I+1].first - (fmt_end + 1)
                : fmt_sv.size() - (fmt_end + 1));
        // Ищем разделитель после текущего значения
        constexpr auto pos = src_sv.find(sep, src_start);
        return pos != std::string_view::npos ? pos : src_sv.size();
    }();
    return std::pair{src_start, src_end};
}



template<std::signed_integral T, fixed_string src>
consteval T parse_value_impl() {
    return utils::parse_number<T, src>();
}

template<std::unsigned_integral T, fixed_string src>
consteval T parse_value_impl() {
    static_assert(!utils::is_negative(src.data), "Unable to parse a negative number string into an unsigned type");
    return utils::parse_number<T, src>();
}

template<typename T, fixed_string src>
requires std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<std::string_view>>
consteval T parse_value_impl() {
    return src.data;
}



template<size_t Size>
constexpr bool operator==(const fixed_string<Size>& lhs, const fixed_string<Size>& rhs) {
    return !std::strcmp(lhs.data, rhs.data);
}



// Семейство функций parse_value
template <typename T, fixed_string spec, fixed_string src>
requires (spec == fixed_string("%d"))
consteval T parse_value() {
    static_assert(utils::is_number(src.data), "Source string is not a valid number");
    static_assert(utils::is_in_range<T>(src.data), "Number is out of given type range");
    return parse_value_impl<T, src>();
}

template <typename T, fixed_string spec, fixed_string src>
requires (spec == fixed_string("%u"))
consteval T parse_value() {
    static_assert(utils::is_positive_number(src.data), "Source string is not a valid number");
    static_assert(utils::is_in_range<T>(src.data), "Number is out of given type range");
    static_assert(std::is_unsigned_v<T>, "Incorrect type provided");
    return parse_value_impl<T, src>();
}

template <typename T, fixed_string spec, fixed_string src>
requires (spec == fixed_string("%s"))
consteval T parse_value() {
    return parse_value_impl<T, src>();
}

// Шаблонная функция, выполняющая преобразования исходных данных в конкретный тип на основе I-го плейсхолдера
template <typename T, int I, format_string fmt, fixed_string source>
consteval T parse_input() {
    constexpr auto src_positions = get_current_source_for_parsing<I, fmt, source>();
    constexpr auto fmt_positions = fmt.placeholder_positions[I];

    constexpr auto src_start = src_positions.first, src_end = src_positions.second;
    constexpr auto spec_start = fmt_positions.first + 1, spec_end = fmt_positions.second;

    constexpr fixed_string<src_end - src_start + 1> src(source.data + src_start, source.data + src_end);
    constexpr fixed_string<spec_end - spec_start + 1> spec(fmt.fmt.data + spec_start, fmt.fmt.data + spec_end);

    static_assert(spec.size() > 0, "Empty format specifier is not allowed");
    static_assert(spec.size() == 3, "Incorrect format specifier");

    return parse_value<T, spec, src>();
}

} // namespace stdx::details

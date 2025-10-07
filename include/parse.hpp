#pragma once

#include <cstdint>
#include <string_view>
#include <cstring>
#include <type_traits>
#include <concepts>
#include <charconv>

#include "format_string.hpp"
#include "types.hpp"


namespace utils {
    template <typename T>
    consteval std::pair<std::from_chars_result, T> parse_number_impl(const char* str) {
        std::remove_cv_t<T> result{};
        auto len = std::strlen(str);
        auto parse_result = std::from_chars(str, str + len, result);
        return {parse_result, result};
    }

    template <typename T, stdx::details::fixed_string src>
    consteval T parse_number() {
        constexpr auto parse_result = parse_number_impl<T>(src.data);

        static_assert((parse_result.first.ec != std::errc::invalid_argument) && (parse_result.first.ptr == src.data + std::strlen(src.data)), "Source string is not a valid number");
        static_assert(parse_result.first.ec != std::errc::result_out_of_range, "Number is out of given type range");
        static_assert(parse_result.first.ec == std::errc(), "Error occured while parsing source string");

        return parse_result.second;
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
    return parse_value_impl<T, src>();
}

template <typename T, fixed_string spec, fixed_string src>
requires (spec == fixed_string("%u"))
consteval T parse_value() {
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

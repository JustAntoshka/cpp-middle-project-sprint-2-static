#pragma once

#include <charconv>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>
#include <system_error>

#include "format_string.hpp"
#include "types.hpp"

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

consteval bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

template <fixed_string Src>
consteval bool is_number() {
    static_assert(Src.size() > 1, "Invalid number format");

    constexpr char begin = Src.data[0];
    constexpr char end = Src.data[Src.size() - 1];
    if constexpr (begin == '+' || begin == '-') {
        return is_number<{Src.data + 1, Src.data + Src.size() - 2}>();
    }
    
    for(size_t i = 0; i < Src.size(); ++i) {
        if constexpr (!is_digit(Src.data[i])) {
            return false;
        }
    }
    return true;
}

// Семейство функций parse_value
template <fixed_string Src, fixed_string Spec, typename T>
consteval T parse_value() {

    static_assert(Spec.size() > 0, "Empty format specifier is not allowed");
    static_assert(Spec.size() == 3, "Incorrect format specifier");
    static_assert(Spec.data[0] == '%', "Incorrect format specifier");
    static_assert(Spec.data[1] == 'd'
               || Spec.data[1] == 'u'
               || Spec.data[1] == 's', "Incorrect format specifier");

    if constexpr (Spec.data[1] == 'd') {
        // return parse_value(Src);
        return 42;
    }
    if constexpr (Spec.data[1] == 'u') {
        // return parse_value(Src);
        return 3.14;
    }
}

// Шаблонная функция, выполняющая преобразования исходных данных в конкретный тип на основе I-го плейсхолдера
template <typename T, int I, format_string fmt, fixed_string source>
consteval T parse_input() {
    constexpr auto src_positions = get_current_source_for_parsing<I, fmt, source>();
    constexpr auto fmt_positions = fmt.placeholder_positions;

    constexpr auto src_start = src_positions.first, src_end = src_positions.second;
    constexpr auto spec_start = fmt_positions.first + 1, spec_end = fmt_positions.second;

    constexpr fixed_string src{src_start, src_end};
    constexpr fixed_string spec{spec_start, spec_end};

    return parse_value(src, spec);
}

} // namespace stdx::details

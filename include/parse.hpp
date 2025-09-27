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
template <typename T>
consteval T parse_value(auto const& src, auto const& spec) {

    static_assert(spec.size() > 0, "Empty format specifier is not allowed");
    static_assert(spec.size() == 3, "Incorrect format specifier");
    // static_assert(spec.data[0] == '%', "Incorrect format specifier");
    // static_assert(spec.data[1] == 'd'
    //            || spec.data[1] == 'u'
    //            || spec.data[1] == 's', "Incorrect format specifier");

    if (spec.data[1] == 'd') {
        return 42;
    }
    return 0;
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

    return parse_value<T>(src, spec);
}

} // namespace stdx::details

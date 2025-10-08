#pragma once

#include <tuple>

#include "parse.hpp"
#include "format_string.hpp"
#include "types.hpp"

namespace stdx {
    
    template<details::format_string fmt, details::fixed_string source, typename... Ts, size_t... Is>
    consteval std::tuple<Ts...> scan_impl(std::index_sequence<Is...>) {
        return std::tuple<Ts...>{
            details::parse_input<Ts, static_cast<int>(Is), fmt, source>()...
        };
    }
    
    // Главная функция
    template <details::format_string fmt, details::fixed_string source, typename... Ts>
    consteval details::scan_result<Ts...> scan() {
        static_assert(fmt.number_placeholders == sizeof...(Ts), "Incorrect placeholders number");
        return details::scan_result<Ts...>(scan_impl<fmt, source, Ts...>(std::index_sequence_for<Ts...>{}));
    }
    
} // namespace stdx
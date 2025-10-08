#pragma once

#include <expected>
#include <array>
#include "types.hpp"

namespace stdx::details {

// Шаблонный класс для хранения форматирующей строчки и ее особенностей
template <auto str>
class format_string {
public:
    static constexpr auto fmt = str;

    // Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
    static consteval std::expected<size_t, parse_error> get_number_placeholders();

    static constexpr auto opt_number_placeholders = get_number_placeholders();
    static_assert(opt_number_placeholders.has_value(), std::string_view(opt_number_placeholders.error().data));
    static constexpr auto number_placeholders = opt_number_placeholders.value();

    // Функция для получения позиций плейсхолдеров
    template <size_t NPos = number_placeholders>
    static consteval auto get_placeholder_positions();

    static constexpr auto placeholder_positions = get_placeholder_positions();
};

// Пользовательский литерал
template<fixed_string Str>
constexpr auto operator""_fs() {
    return format_string<Str>{};
}


template <auto str>
consteval std::expected<size_t, parse_error> format_string<str>::get_number_placeholders() {
    constexpr size_t N = str.size();
    if (!N)
        return 0;
    size_t placeholder_count = 0;
    size_t pos = 0;
    const size_t size = N - 1; // -1 для игнорирования нуль-терминатора

    while (pos < size) {
        // Пропускаем все символы до '{'
        if (str.data[pos] != '{') {
            ++pos;
            continue;
        }

        // Проверяем незакрытый плейсхолдер
        if (pos + 1 >= size) {
            return std::unexpected(parse_error{"Unclosed last placeholder"});
        }

        // Начало плейсхолдера
        ++placeholder_count;
        ++pos;

        // Проверка спецификатора формата
        if (str.data[pos] == '%') {
            ++pos;
            if (pos >= size) {
                return std::unexpected(parse_error{"Unclosed last placeholder"});
            }

            // Проверяем допустимые спецификаторы
            const char spec = str.data[pos];
            constexpr char valid_specs[] = {'d', 'u', 'f', 's'};
            bool valid = false;

            for (const char s : valid_specs) {
                if (spec == s) {
                    valid = true;
                    break;
                }
            }

            if (!valid) {
                return std::unexpected(parse_error{"Invalid specifier."});
            }
            ++pos;
        }

        // Проверяем закрывающую скобку
        if (pos >= size || str.data[pos] != '}') {
            return std::unexpected(parse_error{"\'}\' hasn't been found in appropriate place"});
        }
        ++pos;
    }
    
    return placeholder_count;
}

template <auto str>
template <size_t NPos>
consteval auto format_string<str>::get_placeholder_positions() {
    std::array<std::pair<size_t, size_t>, NPos> positions{};

    auto pos = 0;
    for(auto i = 0; i < str.size(); ++i) {
        if(str.data[i] == '{') {
            positions[pos].first = i;
        }
        if(str.data[i] == '}') {
            positions[pos++].second = i;
        }
    }
    return positions;
}

} // namespace stdx::details
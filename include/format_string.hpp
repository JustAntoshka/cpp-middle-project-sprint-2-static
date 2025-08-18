#pragma once

#include <expected>

#include "types.hpp"

namespace stdx::details {

// Шаблонный класс для хранения форматирующей строчки и ее особенностей
template <fixed_string str>
class format_string {
    static constexpr auto value = str;

    // Функция для получения количества плейсхолдеров и проверки корректности формирующей строки
    static consteval std::expected<size_t, parse_error> get_number_placeholders() {
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

    static constexpr auto number_placeholders = get_number_placeholders();

    static_assert(number_placeholders.has_value(), "AAAAAAAAAAAAAAAAAAAAAA");

    using placeholder_position = std::pair<size_t, size_t>;

    // Функция для получения позиций плейсхолдеров
    template <size_t NPos = number_placeholders>
    static consteval std::array<placeholder_position, NPos> get_placeholder_positions() {
        std::array<placeholder_position, NPos> positions{};

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

    template <size_t NPos = number_placeholders>
    static constexpr std::array<placeholder_position, NPos> placeholder_positions = get_placeholder_positions();
};
    
} // namespace stdx::details


// Пользовательский литерал
// TODO: move to stdx::details
// TODO: write this for format_string
template<stdx::details::fixed_string S>
constexpr auto operator""_fs() {
    return S.data;
}
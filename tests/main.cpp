#include "scan.hpp"


#include <stddef.h>
#include <limits>
#include <string_view>

using namespace stdx::details;

consteval void test_limits() {
{
    using T = int8_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%d}"_fs, "-128", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%d}"_fs, "-127", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%d}"_fs, "-1", T>().values() == Tuple{-1});
    static_assert(stdx::scan<"{%d}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "1", T>().values() == Tuple{1});
    static_assert(stdx::scan<"{%d}"_fs, "126", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%d}"_fs, "127", T>().values() == Tuple{MAX});
}
{
    using T = int16_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%d}"_fs, "-32768", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%d}"_fs, "-32767", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%d}"_fs, "-1", T>().values() == Tuple{-1});
    static_assert(stdx::scan<"{%d}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "1", T>().values() == Tuple{1});
    static_assert(stdx::scan<"{%d}"_fs, "32766", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%d}"_fs, "32767", T>().values() == Tuple{MAX});
}
{
    using T = int32_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%d}"_fs, "-2147483648", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%d}"_fs, "-2147483647", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%d}"_fs, "-1", T>().values() == Tuple{-1});
    static_assert(stdx::scan<"{%d}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "1", T>().values() == Tuple{1});
    static_assert(stdx::scan<"{%d}"_fs, "2147483646", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%d}"_fs, "2147483647", T>().values() == Tuple{MAX});
}
{
    using T = int64_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%d}"_fs, "-9223372036854775808", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%d}"_fs, "-9223372036854775807", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%d}"_fs, "-1", T>().values() == Tuple{-1});
    static_assert(stdx::scan<"{%d}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "1", T>().values() == Tuple{1});
    static_assert(stdx::scan<"{%d}"_fs, "9223372036854775806", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%d}"_fs, "9223372036854775807", T>().values() == Tuple{MAX});
}
{
    using T = uint8_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%u}"_fs, "0", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%u}"_fs, "1", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%u}"_fs, "254", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%u}"_fs, "255", T>().values() == Tuple{MAX});
}
{
    using T = uint16_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%u}"_fs, "0", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%u}"_fs, "1", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%u}"_fs, "65534", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%u}"_fs, "65535", T>().values() == Tuple{MAX});
}
{
    using T = uint32_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%u}"_fs, "0", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%u}"_fs, "1", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%u}"_fs, "4294967294", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%u}"_fs, "4294967295", T>().values() == Tuple{MAX});
}
{
    using T = uint64_t;
    using Tuple = std::tuple<T>;
    constexpr T MIN = std::numeric_limits<T>::min();
    constexpr T MAX = std::numeric_limits<T>::max();
    static_assert(stdx::scan<"{%u}"_fs, "0", T>().values() == Tuple{MIN});
    static_assert(stdx::scan<"{%u}"_fs, "1", T>().values() == Tuple{MIN + 1});
    static_assert(stdx::scan<"{%u}"_fs, "18446744073709551614", T>().values() == Tuple{MAX - 1});
    static_assert(stdx::scan<"{%u}"_fs, "18446744073709551615", T>().values() == Tuple{MAX});
}
{
    using T = std::string_view;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"{%s}"_fs, "", T>().values() == Tuple{""});
    static_assert(stdx::scan<"{%s}"_fs, "1", T>().values() == Tuple{"1"});
    static_assert(stdx::scan<"{%s}"_fs, "\0", T>().values() == Tuple{"\0"});
    static_assert(stdx::scan<"{%s}"_fs, "0123456789abcdefghijklmnopqrstuvwxyz", T>().values() == Tuple{"0123456789abcdefghijklmnopqrstuvwxyz"});
}
};

consteval void test_leading_zeros() {
{
    using T = int8_t;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"{%d}"_fs, "-0100", T>().values() == Tuple{-100});
    static_assert(stdx::scan<"{%d}"_fs, "-00100", T>().values() == Tuple{-100});
    static_assert(stdx::scan<"{%d}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "00", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "000", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%d}"_fs, "0100", T>().values() == Tuple{100});
    static_assert(stdx::scan<"{%d}"_fs, "00100", T>().values() == Tuple{100});
}
{
    using T = uint8_t;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"{%u}"_fs, "0", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%u}"_fs, "00", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%u}"_fs, "000", T>().values() == Tuple{0});
    static_assert(stdx::scan<"{%u}"_fs, "0100", T>().values() == Tuple{100});
    static_assert(stdx::scan<"{%u}"_fs, "00100", T>().values() == Tuple{100});
}
{
    using T = std::string_view;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"{%s}"_fs, "-0100", T>().values() == Tuple{"-0100"});
    static_assert(stdx::scan<"{%s}"_fs, "-00100", T>().values() == Tuple{"-00100"});
    static_assert(stdx::scan<"{%s}"_fs, "0", T>().values() == Tuple{"0"});
    static_assert(stdx::scan<"{%s}"_fs, "00", T>().values() == Tuple{"00"});
    static_assert(stdx::scan<"{%s}"_fs, "000", T>().values() == Tuple{"000"});
    static_assert(stdx::scan<"{%s}"_fs, "0100", T>().values() == Tuple{"0100"});
    static_assert(stdx::scan<"{%s}"_fs, "00100", T>().values() == Tuple{"00100"});
}
}

consteval void test_empty_args() {
{
    static_assert(stdx::scan<""_fs, "">().values() == std::tuple{});
    static_assert(stdx::scan<"Hello world"_fs, "Hello world">().values() == std::tuple{});
}
}

consteval void test_placeholder_positions() {
{
    using T = int8_t;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"-{%d}"_fs, "--42", T>().values() == Tuple{-42});
    static_assert(stdx::scan<"--{%d}"_fs, "---42", T>().values() == Tuple{-42});
    static_assert(stdx::scan<"{%d}1"_fs, "-421", T>().values() == Tuple{-42});
    static_assert(stdx::scan<"{%d}11"_fs, "-4211", T>().values() == Tuple{-42});
    static_assert(stdx::scan<"-{%d}1"_fs, "--421", T>().values() == Tuple{-42});
    static_assert(stdx::scan<"--{%d}11"_fs, "---4211", T>().values() == Tuple{-42});

    static_assert(stdx::scan<"-{%d}"_fs, "-42", T>().values() == Tuple{42});
    static_assert(stdx::scan<"--{%d}"_fs, "--42", T>().values() == Tuple{42});
    static_assert(stdx::scan<"{%d}1"_fs, "421", T>().values() == Tuple{42});
    static_assert(stdx::scan<"{%d}11"_fs, "4211", T>().values() == Tuple{42});
    static_assert(stdx::scan<"-{%d}1"_fs, "-421", T>().values() == Tuple{42});
    static_assert(stdx::scan<"--{%d}11"_fs, "--4211", T>().values() == Tuple{42});
}
{
    using T = uint8_t;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"-{%u}"_fs, "-42", T>().values() == Tuple{42});
    static_assert(stdx::scan<"--{%u}"_fs, "--42", T>().values() == Tuple{42});
    static_assert(stdx::scan<"{%u}1"_fs, "421", T>().values() == Tuple{42});
    static_assert(stdx::scan<"{%u}11"_fs, "4211", T>().values() == Tuple{42});
    static_assert(stdx::scan<"-{%u}1"_fs, "-421", T>().values() == Tuple{42});
    static_assert(stdx::scan<"--{%u}11"_fs, "--4211", T>().values() == Tuple{42});
}
{
    using T = std::string_view;
    using Tuple = std::tuple<T>;
    static_assert(stdx::scan<"-{%s}"_fs, "--42", T>().values() == Tuple{"-42"});
    static_assert(stdx::scan<"--{%s}"_fs, "---42", T>().values() == Tuple{"-42"});
    static_assert(stdx::scan<"{%s}1"_fs, "-421", T>().values() == Tuple{"-42"});
    static_assert(stdx::scan<"{%s}11"_fs, "-4211", T>().values() == Tuple{"-42"});
    static_assert(stdx::scan<"-{%s}1"_fs, "--421", T>().values() == Tuple{"-42"});
    static_assert(stdx::scan<"--{%s}11"_fs, "---4211", T>().values() == Tuple{"-42"});

    static_assert(stdx::scan<"-{%s}"_fs, "-42", T>().values() == Tuple{"42"});
    static_assert(stdx::scan<"--{%s}"_fs, "--42", T>().values() == Tuple{"42"});
    static_assert(stdx::scan<"{%s}1"_fs, "421", T>().values() == Tuple{"42"});
    static_assert(stdx::scan<"{%s}11"_fs, "4211", T>().values() == Tuple{"42"});
    static_assert(stdx::scan<"-{%s}1"_fs, "-421", T>().values() == Tuple{"42"});
    static_assert(stdx::scan<"--{%s}11"_fs, "--4211", T>().values() == Tuple{"42"});
}
}

consteval void test_placeholder_combinations() {
{
    using D = int32_t;
    using U = uint32_t;
    using S = std::string_view;

    static_assert(stdx::scan<"{%d} {%d}"_fs, "-42 42", D, D>().values() == std::tuple<D, D>{-42, 42});
    static_assert(stdx::scan<"{%d} {%u}"_fs, "-42 42", D, U>().values() == std::tuple<D, U>{-42, 42});
    static_assert(stdx::scan<"{%d} {%s}"_fs, "-42 42", D, S>().values() == std::tuple<D, S>{-42, "42"});

    static_assert(stdx::scan<"{%u} {%d}"_fs, "42 -42", U, D>().values() == std::tuple<U, D>{42, -42});
    static_assert(stdx::scan<"{%u} {%u}"_fs, "42 42", U, U>().values() == std::tuple<U, U>{42, 42});
    static_assert(stdx::scan<"{%u} {%s}"_fs, "42 -42", U, S>().values() == std::tuple<U, S>{42, "-42"});

    static_assert(stdx::scan<"{%s} {%d}"_fs, "-42 -42", S, D>().values() == std::tuple<S, D>{"-42", -42});
    static_assert(stdx::scan<"{%s} {%u}"_fs, "-42 42", S, U>().values() == std::tuple<S, U>{"-42", 42});
    static_assert(stdx::scan<"{%s} {%s}"_fs, "-42 -42", S, S>().values() == std::tuple<S, S>{"-42", "-42"});
}
}

int main() {
    test_limits();
    test_leading_zeros();
    test_empty_args();
    test_placeholder_positions();
    test_placeholder_combinations();
    return 0;
}
#include <doctest/doctest.h>
#include "ptcore/parser.h"

#include <type_traits>
#include <array>
#include <easy/test/type_list.h>
#include "tests/fixtures/parse_results_fixture.h"

TEST_CASE("parser types")
{
    using ptcore::parse_input_t;
    using ptcore::parse_results;
    using ptcore::parse_return_t;
    using ptcore::parser;
    using ptcore::parser_parse_type;
    using ptcore::parser_results_type;
    using ptcore::parser_return_type;

    static_assert(std::is_same_v<parse_input_t, std::string_view>);
    static_assert(
        std::is_same_v<parse_return_t<int>, std::optional<parse_results<int>>>);

    auto p1 = [](std::string_view i) { return parse_return_t<int>{}; };
    using p1_type = decltype(p1);
    static_assert(parser<p1_type>);

    auto p2 = [](std::string_view i) { return 12; };
    using p2_type = decltype(p2);
    static_assert(parser<p2_type> == false);

    static_assert(
        std::is_same_v<parser_return_type<p1_type>, parse_return_t<int>>);
    static_assert(
        std::is_same_v<parser_results_type<p1_type>, parse_results<int>>);
    static_assert(std::is_same_v<parser_parse_type<p1_type>, int>);
}

TEST_CASE_FIXTURE(ptcore::test::parse_results_fixture, "parse_results")
{
    using ptcore::parse_results;
    using test_type = parse_results<int>;

    parse_results<int> pr;
    REQUIRE(std::is_same_v<int, decltype(pr)::parse_type>);
    REQUIRE(std::is_same_v<int, decltype(pr)::value_type>);

    SUBCASE("constructors")
    {
        SUBCASE("default constructor")
        {
            test_type tt;
            REQUIRE(tt.value == 0);
            REQUIRE(tt.input_done());
        }

        SUBCASE("copy constructor")
        {
            test_type tt{ base };
            REQUIRE(tt == base);
        }
    }

    SUBCASE("operators")
    {
        SUBCASE("copy assignment operator")
        {
            test_type tt;
            tt = base;
            REQUIRE(tt == base);
        }

        SUBCASE("comparison operators")
        {
            for (int i = 0; const auto& [rhs, expected] : comparisons)
            {
                CAPTURE(i++);

                REQUIRE(expected.compare_equal(base, rhs));
                REQUIRE(expected.compare_not_equal(base, rhs));
            }
        }
    }

    SUBCASE("input_done")
    {
        REQUIRE(pr.input_done());
        pr.remaining_input = "Hi";
        REQUIRE_FALSE(pr.input_done());
    }
}

namespace
{
    // clang-format off
    // list of types that return true for the type trait
    using valid_is_parse_return_type_types = std::tuple
    <
        ptcore::parse_return_t<int>
    >;

    using is_parse_return_type_test_types = easy::tuple_cat_t
    <
        easy::test::primary_types,
        valid_is_parse_return_type_types
    >;
    // clang-format on
}

TEST_CASE_TEMPLATE_DEFINE("detail::is_parse_return_type", TestType, is_parse_return_type_test_id)
{
    using ptcore::detail::is_parse_return_type;
    using ptcore::detail::is_parse_return_type_v;

    if constexpr (easy::is_cv_qualifiable_v<TestType>)
    {
        using qts_t = easy::cv_qualified_set_t<TestType>;

        easy::tuple_enumerate_types<qts_t>([]<auto I, typename T>()
        {
            CAPTURE(I);

            // clang-format off
            constexpr bool expected = easy::tuple_contains_type_v
            <
                T,
                valid_is_parse_return_type_types
            >;
            // clang-format on

            static_assert(is_parse_return_type<T>::value == expected);
            static_assert(is_parse_return_type_v<T> == expected);
        });
    }
    else
    {
        static_assert(is_parse_return_type<TestType>::value == false);
        static_assert(is_parse_return_type_v<TestType> == false);
    }
}
TEST_CASE_TEMPLATE_APPLY(is_parse_return_type_test_id, is_parse_return_type_test_types);

namespace
{
    constexpr auto match_digit()
    {
        return [=](ptcore::parse_input_t s) -> ptcore::parse_return_t<int>
        {
            if (!s.empty())
            {
                if (const auto ch = s.front(); ch >= '0' && ch <= '9')
                {
                    return ptcore::parse_results
                    {
                        ch - '0',
                        s.substr(1)
                    };
                }
            }
    
            return std::nullopt;
        };
    }

    constexpr auto match_digit_separator()
    {
        return [=](ptcore::parse_input_t s)
                   -> ptcore::parse_return_t<ptcore::parse_input_t>
        {
            if (!s.empty())
            {
                if (const auto pos = s.find_first_not_of(" |");
                    pos == ptcore::parse_input_t::npos)
                {
                    return ptcore::parse_results{s, s.substr(s.size())};
                }
                else if (pos > 0)
                {
                    return ptcore::parse_results{s.substr(0, pos),
                                                 s.substr(pos)};
                }
            }

            return std::nullopt;
        };
    }
}

TEST_CASE("match_entirety")
{
    using ptcore::match_entirety;
    using ptcore::parse_input_t;
    using ptcore::parse_return_t;
    using ptcore::parse_results;

    auto p = match_entirety(match_digit());

    REQUIRE(p("0").has_value());
    REQUIRE(p("0").value().value == 0);
    REQUIRE(p("0a") == std::nullopt);
}

TEST_CASE("match_n")
{
    using namespace std::string_view_literals;
    using ptcore::match_n_count;
    using ptcore::match_n;

    SUBCASE("match_n_count")
    {
        std::array test_values =
        {
            std::tuple{ ""sv, 0, false },
            std::tuple{ "C"sv, 0, false },
            std::tuple{ "1"sv, 1, true },
            std::tuple{ "1 "sv, 1, false },
            std::tuple{ "1|"sv, 1, false },
            std::tuple{ "1|C"sv, 1, false },
            std::tuple{ "1|2"sv, 2, true },
            std::tuple{ "1|2 "sv, 2, false },
            std::tuple{ "1|2|"sv, 2, false }
        };

        for (int i = 0; const auto [text, expected_count, expected_full_match] : test_values)
        {
            CAPTURE(i++);

            auto result = match_n_count(match_digit(), match_digit_separator(),
                text);
            REQUIRE(result.count == expected_count);
            REQUIRE(result.full_match == expected_full_match);
        }
    }

    SUBCASE("match_n")
    {
        using ptcore::match_n;
        using namespace std::literals;

        SUBCASE("n is 1")
        {
            using results_t = ptcore::parse_results<std::array<int,1>>;
            using expected_t = ptcore::parse_return_t<std::array<int,1>>;

            constexpr std::array test_values =
            {
                std::tuple{ ""sv, expected_t{std::nullopt} },
                std::tuple{ "C"sv, expected_t{std::nullopt} },
                std::tuple{ "1"sv, expected_t{results_t{std::array<int,1>{ 1 }, ""sv}}},
                std::tuple{ "1 "sv, expected_t{results_t{std::array<int,1>{ 1 }, " "sv}}},
                std::tuple{ "1|"sv, expected_t{results_t{std::array<int,1>{ 1 }, "|"sv}}},
                std::tuple{ "1|C"sv, expected_t{results_t{std::array<int,1>{ 1 }, "|C"sv}}},
                std::tuple{""sv, expected_t{std::nullopt}},
                std::tuple{"d"sv, expected_t{std::nullopt}},
                std::tuple{" D"sv, expected_t{std::nullopt}}
            };

            for (auto i = 0; auto const& [text, expected_value] : test_values)
            {
                CAPTURE(i++);

                auto p = match_n<1>(match_digit(), match_digit_separator());
                REQUIRE(p(text) == expected_value);
            }
        }

        SUBCASE("n is 2")
        {
            using results_t = ptcore::parse_results<std::array<int,2>>;
            using expected_t = ptcore::parse_return_t<std::array<int,2>>;

            constexpr std::array test_values =
            {
                std::tuple{ ""sv, expected_t{std::nullopt} },
                std::tuple{ "C"sv, expected_t{std::nullopt} },
                std::tuple{ "1"sv, expected_t{std::nullopt} },
                std::tuple{ "1|C"sv, expected_t{std::nullopt} },
                std::tuple{ "1|2"sv, expected_t{results_t{std::array<int,2>{ 1, 2 }, ""sv}}},
                std::tuple{ "1|2 "sv, expected_t{results_t{std::array<int,2>{ 1, 2 }, " "sv}}},
                std::tuple{ "1|2|"sv, expected_t{results_t{std::array<int,2>{ 1, 2 }, "|"sv}}},
                std::tuple{ "1|2|C"sv, expected_t{results_t{std::array<int,2>{ 1, 2 }, "|C"sv}}},
                std::tuple{""sv, expected_t{std::nullopt}},
                std::tuple{"d"sv, expected_t{std::nullopt}},
                std::tuple{" D"sv, expected_t{std::nullopt}}
            };

            for (int i = 0; auto const& [text, expected_value] : test_values)
            {
                CAPTURE(i++);

                auto p = match_n<2>(match_digit(), match_digit_separator());
                REQUIRE(p(text) == expected_value);
            }
        }
    }
}

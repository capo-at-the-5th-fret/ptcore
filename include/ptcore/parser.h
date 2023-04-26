#pragma once

#include <string_view>
#include <optional>
#include <concepts>

namespace ptcore
{
    using parse_input_t = std::string_view;

    template <typename T>
    struct parse_results
    {
        using parse_type = T;
        using value_type = parse_type;

        bool operator==(parse_results const&) const = default;

        constexpr bool input_done() const { return remaining_input.empty(); }

        parse_type value{};
        parse_input_t remaining_input;
    };

    template <typename T>
    parse_results(T, parse_input_t) -> parse_results<T>;

    template <typename T>
    using parse_return_t = std::optional<parse_results<T>>;

    namespace detail
    {
        template <typename T>
        struct is_parse_return_type : std::false_type
        {
        };

        template <typename T>
        struct is_parse_return_type<parse_return_t<T>> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_parse_return_type_v =
            is_parse_return_type<T>::value;
    }

    template <typename P>
    concept parser = requires(P p, parse_input_t i)
    {
        p(i);
        requires detail::is_parse_return_type_v<
            std::invoke_result_t<P, parse_input_t>>;
    };

    template <parser P>
    using parser_return_type = std::invoke_result_t<P, parse_input_t>;

    template <parser P>
    using parser_results_type = typename parser_return_type<P>::value_type;

    template <parser P>
    using parser_parse_type = typename parser_results_type<P>::parse_type;

    template <parser P>
    constexpr auto match_entirety(P&& p)
    {
        return [=](parse_input_t s) -> parser_return_type<P>
        {
            if (const auto r = p(s); r && r->input_done())
            {
                return r;
            }

            return std::nullopt;
        };
    }

    struct match_n_count_result
    {
        std::size_t count{0};
        bool full_match{false};
    };

    template <parser P, parser Separator>
    constexpr match_n_count_result match_n_count(P&& p,
                                               Separator&& sep,
                                               parse_input_t s)
    {
        if (const auto r = p(s))
        {
            if (r->input_done())
            {
                return { 1, true };
            }

            s = r->remaining_input;
        }
        else
        {
            return { 0, false };
        }

        match_n_count_result ret{1, false};

        for (;;)
        {
            const auto r = sep(s);
            if (!r)
            {
                break;
            }

            if (const auto r2 = p(r->remaining_input))
            {
                ++ret.count;
                if (r2->input_done())
                {
                    ret.full_match = true;
                    break;
                }

                s = r2->remaining_input;
            }
            else
            {
                break;
            }
        }

        return ret;
    }

    template <std::size_t N, parser P, parser Separator>
    requires(N > 0)
    constexpr auto match_n(P&& p, Separator&& sep)
    {
        using array_t = std::array<parser_parse_type<P>, N>;

        return [=](parse_input_t s) -> parse_return_t<array_t>
        {
            array_t ret;

            if (const auto r = p(s))
            {
                ret[0] = r->value;
                s = r->remaining_input;
            }
            else
            {
                return std::nullopt;
            }

            for (std::size_t i = 1; i < N; ++i)
            {
                const auto r = sep(s);
                if (!r)
                {
                    return std::nullopt;
                }

                if (const auto r2 = p(r->remaining_input))
                {
                    ret[i] = r2->value;
                    s = r2->remaining_input;
                }
                else
                {
                    return std::nullopt;
                }
            }

            return parse_results{std::move(ret), s};
        };
    }
}

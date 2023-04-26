#pragma once

#include <compare>
#include <variant>

enum class equality_result
{
    equal,
    not_equal
};

namespace test
{
    template <typename T>
    struct custom_fixture;

    template <typename... Types>
    struct other_type
    {
    public:
        using variant_t = std::variant<Types...>;

        template <typename T>
        requires std::constructible_from<variant_t, T> other_type(T const& t)
            : value_{t}
        {
        }

        constexpr variant_t const& variant() const { return value_; }

    private:
        std::variant<Types...> value_{};
    };

    template <typename T>
    struct is_other_type : std::false_type
    {
    };

    template <typename... Types>
    struct is_other_type<other_type<Types...>> : std::false_type
    {
    };
}

struct equality_comparator
{
    explicit constexpr equality_comparator(equality_result result)
        : expected(result)
    {
    }

    template <typename T1, typename T2, typename Pred = std::ranges::equal_to>
    constexpr bool compare_equal(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected == equality_result::equal);
    }

    template <typename T, typename... Types>
    constexpr bool compare_equal(T const& lhs,
                                 test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v) {
                return (lhs == v) == (expected == equality_result::equal);
            },
            rhs.variant());
    }

    template <typename T1, typename T2, typename Pred = std::ranges::not_equal_to>
    constexpr bool compare_not_equal(T1 const& lhs, T2 const& rhs,
        Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected == equality_result::not_equal);
    }

    template <typename T, typename... Types>
    constexpr bool compare_not_equal(
        T const& lhs, test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v) {
                return (lhs != v) == (expected == equality_result::not_equal);
            },
            rhs.variant());
    }

    const equality_result expected;
};

inline constexpr equality_comparator equality_equal{equality_result::equal};
inline constexpr equality_comparator equality_not_equal{
    equality_result::not_equal};

enum class ordering_result
{
    less,
    equal,
    greater
};

struct ordering_comparator
{
    explicit constexpr ordering_comparator(ordering_result result)
        : expected(result)
    {
    }

    template <typename T1, typename T2, typename Pred = std::ranges::equal_to>
    constexpr bool compare_equal(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected == ordering_result::equal);
    }

    template <typename T, typename... Types>
    constexpr bool compare_equal(T const& lhs,
                                 test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs == v) == (expected == ordering_result::equal);
            },
            rhs.variant());
    }

    template <typename T1, typename T2, typename Pred = std::ranges::not_equal_to>
    constexpr bool compare_not_equal(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected != ordering_result::equal);
    }

    template <typename T, typename... Types>
    constexpr bool compare_not_equal(
        T const& lhs, test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs != v) == (expected != ordering_result::equal);
            },
            rhs.variant());
    }

    template <typename T1, typename T2, typename Pred = std::ranges::less>
    constexpr bool compare_less(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected == ordering_result::less);
    }

    template <typename T, typename... Types>
    constexpr bool compare_less(T const& lhs,
                                test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs < v) == (expected == ordering_result::less);
            },
            rhs.variant());
    }

    template <typename T1, typename T2, typename Pred = std::ranges::greater>
    constexpr bool compare_greater(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected == ordering_result::greater);
    }

    template <typename T, typename... Types>
    constexpr bool compare_greater(T const& lhs,
                                   test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs > v) == (expected == ordering_result::greater);
            },
            rhs.variant());
    }

    struct le
    {
        template <typename T, typename U>
        requires std::equality_comparable_with<T, U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return std::ranges::less{}(t,u) || std::ranges::equal_to{}(t,u);
        }
    };

    template <typename T1, typename T2, typename Pred = le>//std::ranges::less_equal>
    constexpr bool compare_less_equal(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected != ordering_result::greater);
    }

    template <typename T, typename... Types>
    constexpr bool compare_less_equal(
        T const& lhs, test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs <= v) == (expected != ordering_result::greater);
            },
            rhs.variant());
    }

    struct ge
    {
        template <typename T, typename U>
        requires std::equality_comparable_with<T, U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return std::ranges::greater{}(t,u) || std::ranges::equal_to{}(t,u);
        }
    };

    template <typename T1, typename T2, typename Pred = ge>
    constexpr bool compare_greater_equal(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        return (p(lhs,rhs)) == (expected != ordering_result::less);
    }

    template <typename T, typename... Types>
    constexpr bool compare_greater_equal(
        T const& lhs, test::other_type<Types...> const& rhs) const
    {
        return std::visit(
            [&](auto const& v)
            {
                return (lhs >= v) == (expected != ordering_result::less);
            },
            rhs.variant());
    }

    struct ctw
    {
        template<class T, class U>
        requires std::three_way_comparable_with<T, U> // with different semantic requirements
        constexpr auto operator()(T&& t, U&& u) const
        {

        }
    };

    template <typename T1, typename T2, typename Pred = ctw>//std::ranges::compare_three_way>
    constexpr bool compare_three_way(T1 const& lhs, T2 const& rhs, Pred p = {}) const
    {
        const auto cmp = p(lhs,rhs);

        return (cmp == 0 && expected == ordering_result::equal) ||
            (cmp < 0 && expected == ordering_result::less) ||
            (cmp > 0 && expected == ordering_result::greater);
    }

    const ordering_result expected;
};

inline constexpr ordering_comparator ordering_less{ordering_result::less};
inline constexpr ordering_comparator ordering_equal{ordering_result::equal};
inline constexpr ordering_comparator ordering_greater{ordering_result::greater};

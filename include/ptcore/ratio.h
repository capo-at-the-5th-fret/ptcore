#pragma once

#include <cstdint>
#include <ratio>
#include <type_traits>

namespace ptcore
{
    namespace detail
    {
        template <typename T>
        struct is_ratio_impl : std::false_type
        {
        };

        template <std::intmax_t Num, std::intmax_t Denom>
        struct is_ratio_impl<std::ratio<Num, Denom>> : std::true_type
        {
        };
    }

    template <typename T>
    struct is_ratio : detail::is_ratio_impl<std::remove_cv_t<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_ratio_v = is_ratio<T>::value;

    template <typename T>
    concept ratio = is_ratio_v<T>;
}

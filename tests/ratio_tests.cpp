#include <doctest/doctest.h>
#include "ptcore/ratio.h"

#include <easy/test/type_list.h>

namespace
{
    // clang-format off
    // list of types that return true for the type trait
    using valid_is_ratio_types = std::tuple
    <
        std::ratio<1,2>
    >;

    using is_ratio_test_types = easy::tuple_cat_t
    <
        easy::test::primary_types,
        valid_is_ratio_types
    >;
    // clang-format on
}

TEST_CASE_TEMPLATE_DEFINE("is_ratio", TestType, is_ratio_test_id)
{
    using ptcore::is_ratio;
    using ptcore::is_ratio_v;

    if constexpr (easy::is_cv_qualifiable_v<TestType>)
    {
        using qts_t = easy::cv_qualified_set_t<TestType>;

        easy::tuple_enumerate_types<qts_t>([]<auto I, typename T>()
        {
            CAPTURE(I);

            // clang-format off
            constexpr bool expected = easy::tuple_contains_type_v
            <
                std::remove_cv_t<T>,
                valid_is_ratio_types
            >;
            // clang-format on

            static_assert(is_ratio<T>::value == expected);
            static_assert(is_ratio_v<T> == expected);
        });
    }
    else
    {
        static_assert(is_ratio<TestType>::value == false);
        static_assert(is_ratio_v<TestType> == false);
    }
}
TEST_CASE_TEMPLATE_APPLY(is_ratio_test_id, is_ratio_test_types);

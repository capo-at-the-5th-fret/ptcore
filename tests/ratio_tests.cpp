#include <doctest/doctest.h>
#include "ptcore/ratio.h"

#include <easy/test/type_list.h>

namespace
{
    // list of types that return true for the type trait
    using ratio_type = std::ratio<1,2>;

    using is_ratio_types =
        easy::tuple_append_t<easy::test::primary_types, ratio_type>;
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

            constexpr bool expected = std::is_same_v
            <
                std::remove_cv_t<T>,
                ratio_type
            >;

            static_assert(is_ratio<T>::value == expected);
            static_assert(is_ratio_v<T> == expected);
        });
    }
    else
    {
        static_assert(easy::always_false<TestType>::value == false);
        static_assert(easy::always_false_v<TestType> == false);
    }
}
TEST_CASE_TEMPLATE_APPLY(is_ratio_test_id, is_ratio_types);

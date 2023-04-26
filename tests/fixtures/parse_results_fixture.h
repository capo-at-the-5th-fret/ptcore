#pragma once

#include <tuple>
#include <vector>
#include "tests/compare.h"
#include "ptcore/parser.h"

namespace ptcore::test
{
    struct parse_results_fixture
    {
        parse_results<int> base{ 12, "remaining" };
        parse_results<int> diff{ 13, "remaining2" };

        std::vector<std::tuple<parse_results<int>, equality_comparator>> comparisons =
        {
            { base, equality_equal },
            { parse_results{ diff.value, base.remaining_input }, equality_not_equal },
            { parse_results{ base.value, diff.remaining_input }, equality_not_equal }
        };
    };
}
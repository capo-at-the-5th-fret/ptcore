#include <doctest/doctest.h>
#include "ptcore/text_literals.h"

TEST_CASE("text_literals")
{
    static_assert(ptcore::text_literals::unknown == "?");
}

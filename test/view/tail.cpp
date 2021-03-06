// Range v3 library
//
//  Copyright Eric Niebler 2017-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/single.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    {
        std::vector<int> v{0,1,2,3};
        auto rng = view::tail(v);
        check_equal(rng, {1,2,3});
        CHECK(size(rng) == 3u);
    }

    {
        std::vector<int> v{};
        auto rng = view::tail(v);
        CHECK(empty(rng));
        CHECK(size(rng) == 0u);
    }

    {
        std::stringstream sin{"1 2 3 4"};
        istream_range<int> is(sin);
        auto rng = view::tail(is);
        check_equal(rng, {2,3,4});
    }

    {
        std::stringstream sin{""};
        istream_range<int> is(sin);
        auto rng = view::tail(is);
        CHECK(rng.begin() == rng.end());
    }

    {
        auto rng = view::empty<int>() | view::tail;
        static_assert(0 == size(rng), "");
        CONCEPT_ASSERT(Same<empty_view<int>, decltype(rng)>());
    }

    {
        tail_view<empty_view<int>> const rng(view::empty<int>());
        static_assert(0 == size(rng), "");
    }

    {
        auto const rng = view::single(1) | view::tail;
        static_assert(0 == size(rng), "");
    }

    return ::test_result();
}

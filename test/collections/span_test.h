#include <doctest.h>
#include <vector>

#include "source/fast/collections/span.h"

TEST_SUITE("span") {
    TEST_CASE("span allows iteration over a vector") {
        std::vector<int> numbers {1, 2, 3, 4};

        fast::span<int> s(&*numbers.begin(), &*numbers.end());

        int next = 1;
        for (int i : s) {
            CHECK(next == i);
            next++;
        }
    }
    TEST_CASE("unique_span destroys data") {
        static int alive = 0;
        struct counted {
            counted() { alive++; }
            ~counted() { alive--; }
        };

        {
            fast::unique_span<counted> s(5);
            CHECK(alive == 5);
        }
        CHECK(alive == 0);
    }
}

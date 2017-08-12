#include <doctest.h>
#include <vector>

#include "source/utility/unique_link.h"

TEST_SUITE("unique_link") {
    TEST_CASE("destruction of unique_link unlinks other") {
        fast::unique_link a;
        CHECK(&*a == nullptr);

        {
            fast::unique_link b;
            a.link(b);
            CHECK(&*a == &b);
            CHECK(&a == &*b);
        }

        CHECK(&*a == nullptr);
    }

    TEST_CASE("linked object follow assignment") {
        fast::unique_link a, b, c;

        a.link(b);
        CHECK(&*a == &b);
        CHECK(&a == &*b);

        c = std::move(b);
        CHECK(&*a == &c);
        CHECK(&a == &*c);
        CHECK(&*b == nullptr);
    }

    TEST_CASE("unique_link follows entry in vector after resize") {
        fast::unique_link a;
        std::vector<fast::unique_link> vector;

        vector.resize(4);

        a.link(vector[2]);

        vector.resize(16);

        CHECK(&*a - &*vector.begin() == 2);
    }
}

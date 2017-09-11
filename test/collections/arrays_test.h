#include <doctest.h>

#include "source/fast/collections/arrays.h"

TEST_SUITE("arrays") {
    TEST_CASE("insert should store element") {
        fast::arrays<int, int> a;

        for (int i = 0; i < 10; i++) {
            a.insert(std::make_tuple(i, i + 1));
        }

        CHECK(a.size() == 10);

        int next = 0;
        for (auto i : a) {
            CHECK(std::get<0>(i) == next);
            CHECK(std::get<1>(i) == next + 1);
            next++;
        }
    }

    TEST_CASE("erase should remove element") {
        fast::arrays<int, int> a;

        for (int i = 0; i < 10; i++) {
            a.insert(std::make_tuple(i, i + 1));
        }

        a.erase(a.begin() + 5);
        CHECK(a.size() == 9);

        for (auto i : a) {
            CHECK(std::get<0>(i) != 5);
            CHECK(std::get<1>(i) != 5 + 1);
        }
    }

    TEST_CASE("erase should work with non-copyable types") {
        fast::arrays<std::unique_ptr<int>> a;

        for (int i = 0; i < 10; i++) {
            a.insert(std::make_tuple(std::unique_ptr<int>(new int(5))));
        }

        a.erase(a.begin() + 5);
        CHECK(a.size() == 9);
    }
}

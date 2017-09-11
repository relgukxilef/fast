#include <doctest.h>
#include <vector>

#include "source/fast/collections/unordered_vector.h"

TEST_SUITE("span") {
    TEST_CASE("insert should store element") {
        fast::unordered_vector<int> v;

        CHECK(v.size() == 0);

        auto i = v.insert(5);

        CHECK(v.size() == 1);
        CHECK(*i == 5);
        CHECK(*v.begin() == 5);
    }

    TEST_CASE("deleting the handle should remove element") {
        fast::unordered_vector<int> v;

        {
            auto i = v.insert(5);

            CHECK(v.size() == 1);
        }

        CHECK(v.size() == 0);
    }

    TEST_CASE("reassigning handle should remove element") {
        fast::unordered_vector<int> v;

        auto i = v.insert(5);
        i = v.insert(7);

        CHECK(v.size() == 1);
        CHECK(*i == 7);
        CHECK(*v.begin() == 7);
    }
}

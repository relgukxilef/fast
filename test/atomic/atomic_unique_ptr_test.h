#include <doctest.h>

#include "source/atomic/atomic_unique_ptr.h"

TEST_SUITE("atomic_unique_ptr") {
    static int count = 0;

    struct test {
        test() { count++; }
        ~test() { count--; }
    };

    TEST_CASE("store should delete old value") {
        fast::atomic_unique_ptr<test> pointer(new test());

        CHECK(count == 1);

        pointer.store(nullptr);

        CHECK(count == 0);
    }

    TEST_CASE("store with current value should not delete anything") {
        test* t = new test();

        fast::atomic_unique_ptr<test> pointer(t);

        CHECK(count == 1);

        pointer.store(t);

        CHECK(count == 1);
    }

    TEST_CASE("going out of scope should destroy value pointed to") {
        {
            fast::atomic_unique_ptr<test> pointer(new test());

            CHECK(count == 1);
        }

        CHECK(count == 0);
    }

    TEST_CASE("const_data should point to internal pointer") {
        fast::atomic_unique_ptr<test> pointer;
        std::atomic<test*> const* const_data = pointer.const_data();

        test* t = new test();
        pointer.store(t);
        CHECK(const_data->load() == t);

        t = new test();
        pointer.store(t);
        CHECK(const_data->load() == t);
    }
}

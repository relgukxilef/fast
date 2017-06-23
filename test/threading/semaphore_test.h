#include <doctest.h>

#include <thread>
#include <atomic>

#include "source/threading/semaphore.h"

TEST_SUITE("semaphore") {
    TEST_CASE("wait should wait for signal") {
        fast::semaphore a, b;
        bool signaled_a(false), signaled_b(false);

        std::thread waiter([&]() {
            a.wait();
            CHECK(signaled_a == true);

            CHECK(signaled_b == false);
            signaled_b = true;
            b.signal();
        });

        CHECK(signaled_a == false);
        signaled_a = true;
        a.signal();

        b.wait();
        CHECK(signaled_b == true);

        waiter.join();
    }
}

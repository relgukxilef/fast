#include <doctest.h>

#include "source/atomic/atomic_push_queue.h"

TEST_SUITE("atomic_push_queue") {
    TEST_CASE("pop empty queue should not return false") {
        fast::atomic_push_queue<int> queue;

        int number = 5;
        bool result = queue.pop(number);
        CHECK(result == false);
        CHECK(number == 5);
    }

    TEST_CASE("pop after push should return elements in order") {
        fast::atomic_push_queue<int> queue;

        queue.push(1);
        queue.push(2);

        int number;

        bool result = queue.pop(number);
        CHECK(result == true);
        CHECK(number == 1);

        result = queue.pop(number);
        CHECK(result == true);
        CHECK(number == 2);

        result = queue.pop(number);
        CHECK(result == false);
    }
}

#include <doctest.h>

#include "source/fast/threading/inter_thread_queue.h"

TEST_SUITE("inter_thread_queue") {
    TEST_CASE("pop should retreive elements in correct order") {
        fast::inter_thread_queue<int> queue;

        for (int i = 0; i < 100; i++) {
            queue.push(i);
        }

        for (int i = 0; i < 100; i++) {
            CHECK(queue.top() == i);
            queue.pop();
        }
    }

    TEST_CASE("push should return whether the queue had elements before") {
        fast::inter_thread_queue<int> queue;

        CHECK(queue.push(1) == false);
        CHECK(queue.push(2) == true);

        queue.pop();

        CHECK(queue.push(3) == true);

        queue.pop();
        queue.pop();

        CHECK(queue.push(4) == false);
        CHECK(queue.push(5) == true);
    }

    TEST_CASE("pop should return whether the queue has more elements") {
        fast::inter_thread_queue<int> queue;

        queue.push(1);
        queue.push(2);

        CHECK(queue.pop() == true);
        CHECK(queue.pop() == false);

        queue.push(3);

        CHECK(queue.pop() == false);
    }
}

#include <doctest.h>

#include "source/threading/inter_thread_queue.h"

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

    TEST_CASE("pop should retreive elements from producer") {
        fast::inter_thread_queue<int> queue;

        fast::inter_thread_queue<int>::producer producer(&queue);

        CHECK(producer.push(1) == false);
        CHECK(producer.push(2) == true);

        CHECK(queue.top() == 1);
        queue.pop() == true;
        CHECK(queue.top() == 2);
        queue.pop();
    }

    TEST_CASE("consumer should retreive elements after push") {
        fast::inter_thread_queue<int> queue;

        fast::inter_thread_queue<int>::consumer consumer(&queue);

        queue.push(1);
        queue.push(2);

        CHECK(consumer.top() == 1);
        CHECK(consumer.pop() == true);
        CHECK(consumer.top() == 2);
        CHECK(consumer.pop() == false);
    }
}

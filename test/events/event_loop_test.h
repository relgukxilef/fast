#include <doctest.h>

#include <thread>
#include <atomic>
#include <memory>

#include "source/events/event_loop.h"

TEST_SUITE("event_loop") {
    TEST_CASE("queued events should be executed") {
        bool executed = false;

        fast::event_loop loop;

        auto event_producer = loop.make_event_producer();

        std::shared_ptr<std::atomic<fast::event_loop*>> owner =
            std::make_shared<std::atomic<fast::event_loop*>>(&loop);

        event_producer.push({
            std::weak_ptr<std::atomic<fast::event_loop*>>(owner),
            [&]() {
                executed = true;
                loop.stop();
            }
        });

        loop.run();

        CHECK(executed == true);
    }
}

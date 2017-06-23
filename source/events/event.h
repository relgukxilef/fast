#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <atomic>
#include <functional>

namespace fast {

struct event_loop;

struct event {
    event() = default;

    std::weak_ptr<std::atomic<event_loop*>> owner;
    std::function<void()> function;
};

}

#endif // EVENT_H

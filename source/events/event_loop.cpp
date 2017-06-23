#include <queue>

#include "event_loop.h"

namespace fast {

event_loop::event_loop() : running(true) {
    ;
}

event_loop::~event_loop() {
    ;
}

void event_loop::run() {
    // TODO: if we wanna order the events they need an operator <
    while (running.load(std::memory_order_relaxed)) {
        s.wait();

        std::unique_ptr<inter_thread_queue<event>> new_queue;
        while (new_queues.pop(new_queue)) {
            queues.push_back(std::move(new_queue));
        }

        for (auto& queue : queues) {
            // TODO: testing get_available shouldn't be necessary
            if (queue->get_available() > 0) {
                do {
                    event next_event = std::move(queue->top());

                    // TODO: this check could be moved into event::function
                    auto lock = next_event.owner.lock();
                    auto owner = lock->load(std::memory_order_relaxed);

                    if (owner != this) {
                        auto p = producers.find(owner);

                        if (p == producers.end()) {
                            p = producers.insert(
                                {owner, owner->make_event_producer()}
                            ).first;
                        }

                        p->second.push(std::move(next_event));

                    } else {
                        next_event.function();
                    }
                } while (queue->pop());
            }
        }
    }
}

void event_loop::stop() {
    running.store(false, std::memory_order_relaxed);
}

bool event_loop::is_running() {
    return running.load(std::memory_order_relaxed);
}

event_loop::event_producer event_loop::make_event_producer() {
    inter_thread_queue<event>* new_queue = new inter_thread_queue<event>();
    new_queues.push(std::unique_ptr<inter_thread_queue<event>>(new_queue));
    return event_producer(
        this, inter_thread_queue<event>::producer(new_queue)
    );
}

bool event_loop::event_producer::push(event &&e) {
    bool awake = p.push(e);

    if (!awake) {
        l->s.signal();
    }

    return awake;
}

}

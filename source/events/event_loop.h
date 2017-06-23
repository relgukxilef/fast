#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>

#include "../threading/inter_thread_queue.h"
#include "../threading/semaphore.h"
#include "../atomic/atomic_push_queue.h"
#include "event.h"

namespace fast {

struct event_loop {
    struct event_producer {
        friend struct event_loop;

        bool push(event&& e);

    private:
        event_producer(
            event_loop* l, inter_thread_queue<event>::producer p
        ) : l(l), p(p) {}

        event_loop* l;
        inter_thread_queue<event>::producer p;
    };

    event_loop();
    ~event_loop();

    void run();
    void stop();

    /**
     * @brief is_running
     * @return whether it will process events on next call to run.
     * Does not mean that it is currently in the run method.
     */
    bool is_running();

    event_producer make_event_producer();

private:
    std::vector<std::unique_ptr<inter_thread_queue<event>>> queues;
    atomic_push_queue<std::unique_ptr<inter_thread_queue<event>>> new_queues;

    std::unordered_map<event_loop*, event_producer> producers;

    semaphore s;
    std::atomic_bool running;
};

}

#endif // EVENT_LOOP_H

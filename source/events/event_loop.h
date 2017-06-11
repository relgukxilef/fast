#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <vector>
#include <memory>

#include "../threading/inter_thread_queue.h"
#include "../atomic/atomic_push_queue.h"
#include "event.h"

namespace fast {

struct event_loop {
    event_loop();
    ~event_loop();

    void run();
    void stop();
    bool is_running();

    inter_thread_queue<event>::producer event_producer();

private:
    std::vector<std::unique_ptr<inter_thread_queue<event>>> queues;
    atomic_push_queue<std::unique_ptr<inter_thread_queue<event>>> new_queues;
};

}

#endif // EVENT_LOOP_H

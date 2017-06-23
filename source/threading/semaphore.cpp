#include "semaphore.h"

#include <chrono>

namespace fast {

void semaphore::signal() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        s++;
    }
    condition.notify_one();
}

void semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [&]{return s != 0;});
    s--;
}

void semaphore::wait_for(int milliseconds) {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait_for(lock, std::chrono::milliseconds(milliseconds), [&]{return s != 0;});
    s--;
}

}

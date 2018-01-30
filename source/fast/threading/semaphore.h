#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace fast {

struct semaphore {
    semaphore(int s = 0);

    void signal();
    void wait();
    void wait_for(int milliseconds);

private:
    int s;

    std::mutex mutex;
    std::condition_variable condition;
};

inline semaphore::semaphore(int s) : s(s) { }

inline void semaphore::signal() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        s++;
    }
    condition.notify_one();
}

inline void semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [&]{return s != 0;});
    s--;
}

inline void semaphore::wait_for(int milliseconds) {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait_for(lock, std::chrono::milliseconds(milliseconds), [&]{return s != 0;});
    s--;
}

}

#endif // SEMAPHORE_H

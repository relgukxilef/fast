#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

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

}

#endif // SEMAPHORE_H

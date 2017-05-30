#ifndef ATOMIC_UNIQUE_PTR_H
#define ATOMIC_UNIQUE_PTR_H

#include <atomic>

namespace fast {

template<class T>
struct atomic_unique_ptr {
    atomic_unique_ptr();
    atomic_unique_ptr(T* pointer);

    atomic_unique_ptr(atomic_unique_ptr const&) = delete;

    ~atomic_unique_ptr();

    atomic_unique_ptr& operator=(atomic_unique_ptr const&) = delete;

    T* load(std::memory_order order = std::memory_order_seq_cst) const noexcept;
    void store(
        T* pointer, std::memory_order order = std::memory_order_seq_cst
    ) noexcept;
    bool compare_exchange_weak(T*& expected, T* desired);

    std::atomic<T*> const& const_data() const noexcept;

private:
    std::atomic<T*> pointer;
};

template<class T>
atomic_unique_ptr<T>::atomic_unique_ptr() : pointer(nullptr) {
}

template<class T>
atomic_unique_ptr<T>::atomic_unique_ptr(T* pointer) : pointer(pointer) {
}

template<class T>
atomic_unique_ptr<T>::~atomic_unique_ptr() {
    T* pointer = this->pointer.load();
    if (pointer != nullptr) {
        delete pointer;
    }
}

template<class T>
T* atomic_unique_ptr<T>::load(std::memory_order order) const noexcept {
    return pointer.load(order);
}

template<class T>
void atomic_unique_ptr<T>::store(T* desired, std::memory_order order) noexcept {
    T* old = pointer.exchange(desired, order);
    if (old != nullptr && old != desired) {
        delete old;
    }
}

template<class T>
bool atomic_unique_ptr<T>::compare_exchange_weak(T*& expected, T* desired) {
    return pointer.compare_exchange_weak(expected, desired);
}

template<class T>
typename std::atomic<T*> const&
atomic_unique_ptr<T>::const_data() const noexcept {
    return pointer;
}

}

#endif // ATOMIC_UNIQUE_PTR_H

#ifndef INTER_THREAD_QUEUE_H
#define INTER_THREAD_QUEUE_H

#include <atomic>
#include <cassert>
#include <utility>
#include <memory>

namespace fast {

template<class Item>
struct inter_thread_queue {
    /* Thread-safe, dynamically growing queue.
     * When an element has to be inserted when the
     * available space is not sufficient alocate more
     */

    inter_thread_queue(int capacity = 4);
    ~inter_thread_queue();

    inter_thread_queue(const inter_thread_queue &) = delete;

    /**
     * @brief push an element to the queue
     * @param value The value to push
     * @return true if the queue was not empty, false if it was
     */
    bool push(Item&& value);
    bool push(Item const& value);

    /**
     * @brief pop an element off the queue
     * @return true if the queue has more elements
     */
    bool pop();

    Item &top();

    int available();

private:
    /* TODO: instead of size store the number of items
     * written and read. This can be done using only
     * load and store which are much cheaper.
     * It's easily possible for the using class to
     * determine whether it should call wait/notify.
     * Just have an atomic_bool that's set before every
     * wait and cleared after every notify.
     */

    struct block {
        const unsigned int size;
        const std::unique_ptr<Item> items;
        block *next; // may only be written during insertion

        block(unsigned int size, block *next = nullptr);
        ~block() = default;
    };

    // number of elements
    std::atomic_uint size;
    // number of blocks with elements
    std::atomic_uint block_size;

    // producer variables:
    // number of elemts that can be stored without allocatation
    unsigned int capacity;
    // number of blocks allocated
    unsigned int block_capacity;
    // index to write next in head Block
    unsigned int write;
    block *head;

    // consumer variables:
    // index to read next in tail Block
    unsigned int read;
    block *tail;
};

template<class Item>
inter_thread_queue<Item>::block::block(
    unsigned int size, inter_thread_queue<Item>::block *next
) :
    size(size), items(new Item[size]), next(next ? next : this) {}

template<class Item>
inter_thread_queue<Item>::inter_thread_queue(int capacity) :
    size(0),
    block_size(1),
    capacity(capacity),
    block_capacity(1),
    write(0),
    head(new block(capacity)),
    read(0),
    tail(head) {
}

template<class Item>
inter_thread_queue<Item>::~inter_thread_queue() {
    if (head != nullptr) {
        block *current = head;
        do {
            block *next = current->next;
            delete current;
            current = next;
        } while (current != head);
    }
}

template<class Item>
bool inter_thread_queue<Item>::push(Item &&value) {
    if (write >= head->size) {
        // no space left in this block
        if (block_size.load(std::memory_order_relaxed) >= block_capacity) {
            // the next block is not available
            // double the capacity
            head->next = new block(capacity, head->next);
            capacity += capacity;
            block_capacity++;
        }

        write = 0;
        head = head->next;
        block_size.fetch_add(1, std::memory_order_relaxed);
    }

    head->items.get()[write] = std::move(value);
    bool empty = size.fetch_add(1, std::memory_order_release) == 0;
    write++;

    return !empty;
}

template<class Item>
bool inter_thread_queue<Item>::push(Item const& value) {
    int copy = value;
    return push(std::move(copy));
}

template<class Item>
bool inter_thread_queue<Item>::pop() {
    assert(size.load() > 0);

    bool last = size.fetch_sub(1, std::memory_order_acquire) == 1;
    read++;

    if (read >= tail->size) {
        // done with current block
        tail = tail->next;
        read = 0;
        block_size.fetch_sub(1, std::memory_order_relaxed);
    }

    return !last;
}

template<class Item>
Item &inter_thread_queue<Item>::top() {
    return tail->items.get()[read];
}

}

#endif // INTER_THREAD_QUEUE_H

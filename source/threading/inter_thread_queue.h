#ifndef INTER_THREAD_QUEUE_H
#define INTER_THREAD_QUEUE_H

#include <atomic>
#include <cassert>
#include <utility>
#include <memory>

namespace fast {

/**
 * @brief The inter_thread_queue struct is a dynamically growing queue.
 * It provides a way of communication between two threads.
 * Call to push and pop operations synchronize with each other but
 * multiple calls to push or pop are not thread safe.
 */
template<class Item>
struct inter_thread_queue {
    /**
     * @brief The producer struct represents the end of the queue where
     * elements are inserted. This struct is not thread safe.
     */
    struct producer {
        producer(inter_thread_queue* queue);

        /**
         * @brief push an element to the queue
         * @param value The value to push
         * @return true if the queue was not empty, false if it was
         */
        bool push(Item&& value);
        bool push(Item const& value);

    private:
        inter_thread_queue* queue;
    };

    /**
     * @brief The consumer struct represents the end of the queue where
     * elements are read from. This struct is not thread safe.
     */
    struct consumer {
        consumer(inter_thread_queue* queue);

        /**
         * @brief pop an element off the queue
         * @return true if the queue has more elements
         */
        bool pop();

        /**
         * @brief get the next element in the queue
         * @return the top of the queue
         */
        Item& top();

    private:
        inter_thread_queue* queue;

    };

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

    /**
     * @brief get the next element in the queue
     * @return the top of the queue
     */
    Item& top();

    /**
     * @brief get_available number of elements
     * @return the number of available elements
     */
    int get_available();

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
inter_thread_queue<Item>::producer::producer(inter_thread_queue<Item>* queue) :
    queue(queue)
{
}

template<class Item>
bool inter_thread_queue<Item>::producer::push(Item const& value) {
    return queue->push(value);
}

template<class Item>
bool inter_thread_queue<Item>::producer::push(Item&& value) {
    return queue->push(value);
}

template<class Item>
inter_thread_queue<Item>::consumer::consumer(inter_thread_queue<Item>* queue) :
    queue(queue)
{
}

template<class Item>
bool inter_thread_queue<Item>::consumer::pop() {
    return queue->pop();
}

template<class Item>
Item& inter_thread_queue<Item>::consumer::top() {
    return queue->top();
}

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
Item& inter_thread_queue<Item>::top() {
    return tail->items.get()[read];
}

template<class Item>
int inter_thread_queue<Item>::get_available() {
    return size.load(std::memory_order_acquire);
}

template<class Item>
inter_thread_queue<Item>::block::block(
    unsigned int size, inter_thread_queue<Item>::block* next
) :
    size(size), items(new Item[size]), next(next ? next : this) {}

}

#endif // INTER_THREAD_QUEUE_H

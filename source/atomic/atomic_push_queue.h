#ifndef ATOMIC_LINKED_QUEUE_H
#define ATOMIC_LINKED_QUEUE_H

#include <atomic>
#include <cassert>

#include "atomic_unique_ptr.h"

namespace fast {

template<class Item>
struct atomic_push_queue {
    atomic_push_queue();
    ~atomic_push_queue();

    void push(Item&& i);
    bool pop(Item& i);

private:
    struct node {
        atomic_unique_ptr<node> next;
        Item value;
    };

    // alternate between two linked lists
    atomic_unique_ptr<node> lists[2];
    std::atomic_int readers[2];

    // next to be popped
    atomic_unique_ptr<node>* next;

    // list to push to
    std::atomic_bool index;
};

template<class Item>
atomic_push_queue<Item>::atomic_push_queue() :
    next(lists + 1), index(0)
{
    readers[0] = 0;
    readers[1] = 0;
}

template<class Item>
atomic_push_queue<Item>::~atomic_push_queue() {
}

template<class Item>
void atomic_push_queue<Item>::push(Item&& item) {
    node* new_node = new node {{nullptr}, item};

    bool index = this->index.load();
    int count = readers[index].fetch_add(1);
    assert(count >= 0);

    atomic_unique_ptr<node>* n = &lists[index];
    node* expected = nullptr;

    while (!n->compare_exchange_weak(expected, new_node)) {
        if (expected != nullptr) {
            // compare_exchange_weak may fail spuriously
            n = &expected->next;
            expected = nullptr;
        }
    }

    count = readers[index].fetch_sub(1);
    assert(count >= 1);
}

template<class Item>
bool atomic_push_queue<Item>::pop(Item& item) {
    bool result = false;

    node* n = next->load();
    if (n != nullptr) {
        next = &n->next;
        item = n->value;
        result = true;

    } else {
        bool index = this->index.load();
        int count = readers[!index].fetch_add(1);
        assert(count >= 0);

        if (count == 0) {
            // no thread is reading the inactive list right now
            // delete it
            lists[!index].store(nullptr);
            // make empty list the active list
            this->index.store(!index);

            next = lists + index;

            n = next->load();
            if (n != nullptr) {
                next = &n->next;
                item = n->value;
                result = true;
            }
        }

        count = readers[!index].fetch_sub(1);
        assert(count >= 1);
    }

    return result;
}

}

#endif // ATOMIC_LINKED_QUEUE_H

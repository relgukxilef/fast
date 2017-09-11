#ifndef UNIQUE_LINK_H
#define UNIQUE_LINK_H

#include <memory>

namespace fast {

struct unique_link {
    unique_link();
    unique_link(const unique_link&) = delete;
    unique_link(unique_link&& other);

    ~unique_link();

    unique_link& operator= (const unique_link&) = delete;
    unique_link& operator= (unique_link&& other);

    unique_link& operator* ();
    unique_link* operator-> ();

    void link(unique_link& other);

    void unlink();

private:
    unique_link* pointer;
};


unique_link::unique_link() : pointer(nullptr) {}

unique_link::unique_link(unique_link&& other) : pointer(nullptr) {
    if (other.pointer != nullptr) {
        other.pointer->link(*this);
    }
}

unique_link::~unique_link() {
    unlink();
}

unique_link& unique_link::operator= (unique_link&& other) {
    if (other.pointer != nullptr) {
        other.pointer->link(*this);
    }
    return *this;
}

unique_link& unique_link::operator*() {
    return *pointer;
}

unique_link* unique_link::operator->() {
    return pointer;
}

void unique_link::link(unique_link& other) {
    unlink();
    other.unlink();
    other.pointer = this;
    pointer = &other;
}

void unique_link::unlink() {
    if (pointer != nullptr) {
        pointer->pointer = nullptr;
        pointer = nullptr;
    }
}

}

#endif // UNIQUE_LINK_H

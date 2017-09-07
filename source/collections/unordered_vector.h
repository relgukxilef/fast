#ifndef UNORDERED_VECTOR_H
#define UNORDERED_VECTOR_H

#include <vector>
#include <cassert>

namespace fast {

template<class T>
struct unordered_vector {
private:
    struct reverse_handle;

public:
    friend struct handle;
    struct handle {
        friend struct unordered_vector;
        handle();
        handle(const handle&) = delete;
        handle(handle&& o);
        ~handle();

        handle& operator= (const handle&) = delete;
        handle& operator= (handle&& o);

        T& operator*();
        T* operator->();

    private:
        handle(unordered_vector<T>* parent, reverse_handle* r);

        void unlink();
        void link(handle&& o);

        unordered_vector<T>* parent;
        reverse_handle* r;
    };

    ~unordered_vector();

    handle insert(T&& t);

    T* begin();
    T* end();

    std::size_t size() const;

private:
    struct reverse_handle {
        reverse_handle();
        reverse_handle(const reverse_handle&) = delete;
        reverse_handle(reverse_handle&& o);
        ~reverse_handle();

        reverse_handle& operator=(const reverse_handle&) = delete;
        reverse_handle& operator=(reverse_handle&& o);

        handle* h;
    };

    // TODO: use arrays<T, reverse_handle>
    std::vector<T> elements;
    std::vector<reverse_handle> handles;
};


template<class T>
unordered_vector<T>::handle::handle() : parent(nullptr), r(nullptr) {}

template<class T>
unordered_vector<T>::handle::handle(
    unordered_vector<T>::handle&& o
) : parent(nullptr), r(nullptr) {
    link(std::move(o));
}

template<class T>
unordered_vector<T>::handle::~handle() {
    unlink();
}

template<class T>
typename unordered_vector<T>::handle&
unordered_vector<T>::handle::operator=(
    unordered_vector<T>::handle&& o
) {
    link(std::move(o));

    return *this;
}

template<class T>
T& unordered_vector<T>::handle::operator*() {
    std::size_t index = r - &*parent->handles.begin();
    return parent->elements[index];
}

template<class T>
T* unordered_vector<T>::handle::operator->() {
    return &operator*();
}

template<class T>
unordered_vector<T>::handle::handle(
    unordered_vector<T>* parent, unordered_vector<T>::reverse_handle* r
) : parent(parent), r(r) {}

template<class T>
void unordered_vector<T>::handle::unlink() {
    if (r != nullptr) {
        // remove element associated whith this
        std::size_t index = r - &*parent->handles.begin();
        parent->elements[index] = std::move(parent->elements.back());
        parent->elements.pop_back();
        parent->handles[index] = std::move(parent->handles.back());
        parent->handles.pop_back();
        parent = nullptr;
        r = nullptr;
    }
}

template<class T>
void unordered_vector<T>::handle::link(
    unordered_vector<T>::handle&& o
) {
    unlink();

    if (o.r != nullptr) {
        // move o into this
        parent = o.parent;
        r = o.r;

        r->h = this;

        o.parent = nullptr;
        o.r = nullptr;
    }
}

template<class T>
unordered_vector<T>::~unordered_vector() {
    for (auto& r : handles) {
        r.h->r = nullptr;
    }
}

template<class T>
typename unordered_vector<T>::handle unordered_vector<T>::insert(T&& t) {
    elements.push_back(std::move(t));

    handles.push_back(std::move(reverse_handle()));
    handle h(this, &handles.back());
    handles.back().h = &h;

    return h;
}

template<class T>
T* unordered_vector<T>::begin() {
    return &*elements.begin();
}

template<class T>
T* unordered_vector<T>::end() {
    return &*elements.end();
}

template<class T>
std::size_t unordered_vector<T>::size() const {
    return elements.size();
}

template<class T>
unordered_vector<T>::reverse_handle::reverse_handle() : h(nullptr) {}

template<class T>
unordered_vector<T>::reverse_handle::reverse_handle(
    unordered_vector<T>::reverse_handle&& o
) {
    // only used when initializing new storage with existing elements
    if (o.h != nullptr) {
        h = o.h;
        h->r = this;
        o.h = nullptr;
    }
}

template<class T>
unordered_vector<T>::reverse_handle::~reverse_handle() {
    // gets called both on full containers in desctructor and pop_back of vector
    // and on empty ones when storage is resized <- only case that's important here
    if (h != nullptr) {
        h->r = nullptr;
        h = nullptr;
    }
    //assert(h == nullptr);
}

template<class T>
typename unordered_vector<T>::reverse_handle&
unordered_vector<T>::reverse_handle::operator=(
    unordered_vector<T>::reverse_handle&& o
) {
    // only called in destructor of handle
    if (h != nullptr) {
        h->r = nullptr;
    }

    if (o.h != nullptr) {
        h = o.h;
        h->r = this;
        o.h = nullptr;
    }

    return *this;
}

}

#endif // UNORDERED_VECTOR_H

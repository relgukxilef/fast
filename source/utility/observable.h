#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <functional>

namespace fast {

namespace detail {
    struct ring {
        ring& operator= (ring& other);
        ring& operator= (ring&& other);

        ring* next, * previous;

    protected:
        ring();
        ring(const ring& other) = delete;
        ring(ring& other);
        ring(ring&& other);

        ~ring();
    };
}

template<class Type>
struct observer;

template<class Type>
struct observable : private detail::ring {
    friend struct observer<Type>;

    void set(Type&& new_value);

    observable<Type>& operator= (const observable<Type>& o) = delete;
    observable<Type>& operator= (const Type& new_value);
    observable<Type>& operator= (Type&& new_value);

    operator const Type&() {
        return value;
    }

private:
    Type value;
};

template<class Type>
struct observer : private detail::ring {
    observer(std::function<void(const Type&)> callback);

    observer<Type>& operator= (observable<Type>& o);

    std::function<void(const Type&)> callback;
};


namespace detail {
    ring::ring() : next(this), previous(this) {}
    ring::ring(ring& other) : next(other.next), previous(&other) {
        other.next->previous = this;
        other.next = this;
        // other.previous - other - this - other.next
    }
    ring::ring(ring&& other) : next(other.next), previous(other.previous) {
        other.next->previous = this;
        other.previous->next = this;
        other.previous = &other;
        other.next = &other;
        // other.previous - this - other.next
    }

    ring::~ring() {
        next->previous = previous;
        previous->next = next;
    }

    ring& ring::operator= (ring& other) {
        // remove this from its ring
        next->previous = previous;
        previous->next = next;

        // add this to others ring
        next = other.next;
        previous = &other;
        other.next->previous = this;
        other.next = this;
        // other.previous - other - this - other.next
        return *this;
    }
    ring& ring::operator= (ring&& other) {
        // make this replace other in its ring
        next = other.next;
        previous = other.previous;
        other.next->previous = this;
        other.previous->next = this;
        other.previous = &other;
        other.next = &other;
        // other.previous - this - other.next
        return *this;
    }
}

template<class Type>
void observable<Type>::set(Type&& new_value) {
    value = new_value;
    detail::ring *current = next;
    while (next != this) {
        reinterpret_cast<observer<Type>*>(current)->callback(value);
        next = next->next;
    }
}

template<class Type>
observable<Type>& observable<Type>::operator= (const Type& new_value) {
    set(new_value);
    return *this;
}

template<class Type>
observable<Type>& observable<Type>::operator= (Type&& new_value) {
    set(std::move(new_value));
    return *this;
}

template<class Type>
observer<Type>::observer(std::function<void(const Type&)> callback) :
    callback(callback) {}

template<class Type>
observer<Type>& observer<Type>::operator= (observable<Type>& o) {
    // remove this from current ring and add it to ring of o
    ring::operator= (o);
    return *this;
}

}

#endif // OBSERVABLE_H

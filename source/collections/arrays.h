#ifndef ARRAYS_H
#define ARRAYS_H

#include <tuple>
#include <memory>
#include <iterator>

#include "span.h"
#include "tuple.h"

namespace fast {

template<class... Types>
struct arrays {
    struct iterator : public std::iterator<
        std::bidirectional_iterator_tag, std::tuple<Types&&...>
    > {
        iterator();
        iterator(const std::tuple<Types*...>& data);

        std::tuple<Types&&...> operator*();
        void operator++();
        void operator--();
        iterator operator+(std::ptrdiff_t value);
        iterator operator-(std::ptrdiff_t value);
        std::ptrdiff_t operator-(const iterator& rhs);
        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        std::tuple<Types*...> data;
    };

    template<class Type>
    span<Type> get();

    template<int N>
    typename std::tuple_element<N, std::tuple<Types...>>::type get();

    size_t size() const;
    size_t capacity() const;

    iterator begin() const;
    iterator end() const;

    iterator insert(std::tuple<Types&&...> value);
    iterator erase(iterator i);

private:
    std::tuple<unique_span<Types>...> data;
    iterator end_iterator;
};

namespace detail {
    // functors
    struct incrementer {
        template<class Type>
        Type& operator()(Type& i);
    };

    struct decrementer {
        template<class Type>
        Type& operator()(Type& i);
    };

    struct adder {
        std::ptrdiff_t diff;

        adder(std::ptrdiff_t diff = 0);

        template<class Type>
        Type operator()(const Type& i);
    };

    struct unique_span_beginner {
        template<class Type>
        Type* operator()(const unique_span<Type>& s);
    };

    struct unique_span_ender {
        template<class Type>
        Type* operator()(const unique_span<Type>& s);
    };

    struct pointer_referencer {
        template<class Type>
        Type&& operator()(Type* p);
    };
}


template<class... Types>
arrays<Types...>::iterator::iterator() : data(
    std::make_tuple(static_cast<Types*>(nullptr)...)
) {}

template<class... Types>
arrays<Types...>::iterator::iterator(const std::tuple<Types*...>& data) :
    data(data) {}

template<class... Types>
std::tuple<Types&&...> arrays<Types...>::iterator::operator*() {
    return map(data, detail::pointer_referencer());
}

template<class... Types>
void arrays<Types...>::iterator::operator++() {
    map(data, detail::incrementer());
}

template<class... Types>
void arrays<Types...>::iterator::operator--() {
    map(data, detail::decrementer());
}

template<class... Types>
typename arrays<Types...>::iterator
arrays<Types...>::iterator::operator+(std::ptrdiff_t value) {
    return iterator(map(data, detail::adder(value)));
}

template<class... Types>
typename arrays<Types...>::iterator
arrays<Types...>::iterator::operator-(std::ptrdiff_t value) {
    return iterator(map(data, detail::adder(-value)));
}

template<class... Types>
std::ptrdiff_t arrays<Types...>::iterator::operator-(const iterator& rhs) {
    return std::get<0>(data) - std::get<0>(rhs.data);
}

template<class... Types>
bool arrays<Types...>::iterator::operator==(
    const arrays<Types...>::iterator& rhs
) const {
    return std::get<0>(data) == std::get<0>(rhs.data);
}

template<class... Types>
bool arrays<Types...>::iterator::operator!=(
    const arrays<Types...>::iterator& rhs
) const {
    return std::get<0>(data) != std::get<0>(rhs.data);
}

template<class... Types> template<class Type>
span<Type> arrays<Types...>::get() {
    return span<Type>(std::get<unique_span<Type>>(data));
}

template<class... Types> template<int N>
typename std::tuple_element<N, std::tuple<Types...>>::type
arrays<Types...>::get() {
    return span<typename std::tuple_element<N, std::tuple<Types...>>::type>(
        std::get<N>(data)
    );
}

template<class... Types>
size_t arrays<Types...>::size() const {
    return end() - begin();
}

template<class... Types>
size_t arrays<Types...>::capacity() const {
    return std::get<0>(data).end() - std::get<0>(data).begin();
}

template<class... Types>
typename arrays<Types...>::iterator arrays<Types...>::begin() const {
    return iterator(map(data, detail::unique_span_beginner()));
}

template<class... Types>
typename arrays<Types...>::iterator arrays<Types...>::end() const {
    return end_iterator;
}

template<class... Types>
typename arrays<Types...>::iterator
arrays<Types...>::insert(std::tuple<Types&&...> value) {
    if (size() == capacity()) {
        // resize
        // TODO: allocate storage without initializing elements
        std::tuple<unique_span<Types>...> new_data;
        if (capacity() == 0) {
            new_data = std::make_tuple(unique_span<Types>(4)...);
        } else {
            new_data = std::make_tuple(unique_span<Types>(capacity() * 2)...);
        }

        end_iterator = std::move(
            begin(), end(),
            iterator(map(new_data, detail::unique_span_beginner()))
        );

        data = std::move(new_data);
    }

    *end_iterator = std::move(value);
    iterator i = end_iterator;
    ++end_iterator;
    return i;
}

template<class... Types>
typename arrays<Types...>::iterator
arrays<Types...>::erase(arrays<Types...>::iterator i) {
    --end_iterator;
    *i = std::move(*end_iterator);
    return i;
}

template<class Type>
Type& detail::incrementer::operator()(Type& i) {
    i++;
    return i;
}

template<class Type>
Type& detail::decrementer::operator()(Type& i) {
    i--;
    return i;
}

detail::adder::adder(std::ptrdiff_t diff) : diff(diff) {}

template<class Type>
Type detail::adder::operator()(const Type& i) {
    return i + diff;
}

template<class Type>
Type* detail::unique_span_beginner::operator()(const unique_span<Type>& s) {
    return s.begin();
}

template<class Type>
Type* detail::unique_span_ender::operator()(const unique_span<Type>& s) {
    return s.end();
}

template<class Type>
Type&& detail::pointer_referencer::operator()(Type* p) {

    return std::move(*p);
}

}

#endif // ARRAYS_H

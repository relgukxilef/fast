#ifndef SPAN_H
#define SPAN_H

#include <memory>

namespace fast {

template<class Type>
struct unique_span {
    unique_span();
    unique_span(std::unique_ptr<Type[]>&& begin, Type* end);
    unique_span(Type* begin, Type* end);
    unique_span(size_t size);

    Type* begin() const;
    Type* end() const;

private:
    std::unique_ptr<Type[]> begin_iterator;
    Type* end_iterator;
};

template<class Type>
struct span {
    span();
    span(Type* begin, Type* end);
    span(const unique_span<Type>& o);

    Type* begin() const;
    Type* end() const;

private:
    Type* begin_iterator;
    Type* end_iterator;
};


template<class Type>
unique_span<Type>::unique_span() : end_iterator(nullptr) {}

template<class Type>
unique_span<Type>::unique_span(std::unique_ptr<Type[]>&& begin, Type* end) :
    begin_iterator(std::move(begin)), end_iterator(end) {}

template<class Type>
unique_span<Type>::unique_span(Type* begin, Type* end) :
    begin_iterator(begin), end_iterator(end) {}

template<class Type>
unique_span<Type>::unique_span(size_t size) :
    begin_iterator(new Type[size]), end_iterator(begin_iterator.get() + size) {}

template<class Type>
Type* unique_span<Type>::begin() const {
    return begin_iterator.get();
}

template<class Type>
Type* unique_span<Type>::end() const {
    return end_iterator;
}

template<class Type>
span<Type>::span() : begin_iterator(nullptr), end_iterator(nullptr) {}

template<class Type>
span<Type>::span(Type* begin, Type* end) :
    begin_iterator(begin), end_iterator(end) {}

template<class Type>
span<Type>::span(const unique_span<Type>& o) :
    begin_iterator(o.begin()), end_iterator(o.end()) {}

template<class Type>
Type* span<Type>::begin() const {
    return begin_iterator;
}

template<class Type>
Type* span<Type>::end() const {
    return end_iterator;
}

}

#endif // SPAN_H

#ifndef TUPLE_H
#define TUPLE_H

#include <tuple>
#include <utility>

namespace {
    template<class... Types, class Function, size_t... I>
    auto map_implementation(
        std::tuple<Types...>& input, Function function,
        std::index_sequence<I...>
    ) -> std::tuple<decltype(function(std::get<I>(input)))...> {
        return std::tuple<decltype(function(std::get<I>(input)))...>(
            function(std::get<I>(input))...
        );
    }

    template<class... Types, class Function, size_t... I>
    auto map_implementation(
        const std::tuple<Types...>& input, Function function,
        std::index_sequence<I...>
    ) -> std::tuple<decltype(function(std::get<I>(input)))...> {
        return std::make_tuple(function(std::get<I>(input))...);
    }
}

template<class... Types, class Function>
auto map(std::tuple<Types...>& input, Function function) {
    return map_implementation(
        input, function, std::index_sequence_for<Types...>()
    );
}

template<class... Types, class Function>
auto map(const std::tuple<Types...>& input, Function function) {
    return map_implementation(
        input, function, std::index_sequence_for<Types...>()
    );
}

#endif // TUPLE_H

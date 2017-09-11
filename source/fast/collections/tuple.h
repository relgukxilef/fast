#ifndef TUPLE_H
#define TUPLE_H

#include <tuple>
#include <utility>

namespace fast {

namespace detail {
    template<class... Types, class Function, size_t... I>
    auto map(
        std::tuple<Types...>& input, Function function,
        std::index_sequence<I...>
    ) -> std::tuple<decltype(function(std::get<I>(input)))...>;

    template<class... Types, class Function, size_t... I>
    auto map(
        const std::tuple<Types...>& input, Function function,
        std::index_sequence<I...>
    ) -> std::tuple<decltype(function(std::get<I>(input)))...>;
}

template<class... Types, class Function>
auto map(std::tuple<Types...>& input, Function function);

template<class... Types, class Function>
auto map(const std::tuple<Types...>& input, Function function);


template<class... Types, class Function, size_t... I>
auto detail::map(
    std::tuple<Types...>& input, Function function,
    std::index_sequence<I...>
) -> std::tuple<decltype(function(std::get<I>(input)))...> {
    return std::tuple<decltype(function(std::get<I>(input)))...>(
        function(std::get<I>(input))...
    );
}

template<class... Types, class Function, size_t... I>
auto detail::map(
    const std::tuple<Types...>& input, Function function,
    std::index_sequence<I...>
) -> std::tuple<decltype(function(std::get<I>(input)))...> {
    return std::make_tuple(function(std::get<I>(input))...);
}

template<class... Types, class Function>
auto map(std::tuple<Types...>& input, Function function) {
    return detail::map(
        input, function, std::index_sequence_for<Types...>()
    );
}

template<class... Types, class Function>
auto map(const std::tuple<Types...>& input, Function function) {
    return detail::map(
        input, function, std::index_sequence_for<Types...>()
    );
}

}

#endif // TUPLE_H

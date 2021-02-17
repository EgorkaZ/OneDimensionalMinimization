#pragma once

#include <functional>
#include <type_traits>

namespace min1d {

using CalculateFunc = std::function<double(double)>;

template <class Container>
struct is_container
{
    template <class Cont>
    static auto check(const Cont * c) -> std::integral_constant<bool, std::is_same_v<decltype(c->begin()), decltype(c->begin())>>;
    static auto check(...) -> std::false_type;

    using ContPtr = const Container *;
    static constexpr bool value = decltype(check(ContPtr{}))::value;
};
template <class Container>
inline constexpr bool is_container_v = is_container<Container>::value;

template <class Container, class T>
struct is_container_of : std::integral_constant<bool, std::is_same_v<typename Container::value_type, T>>
{};
template <class Container, class T>
inline constexpr bool is_container_of_v = is_container_of<Container, T>::value;

}

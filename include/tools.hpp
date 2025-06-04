#pragma once
#ifndef ROTVI_TOOLS_HPP
#define ROTVI_TOOLS_HPP

#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <type_traits>
#include <typeinfo>

#define nop void(0)

inline std::time_t timmer = 0;
inline std::size_t counter = 0;
inline std::fstream *logStream;

#define timmerOn                                                                                                       \
    {                                                                                                                  \
        timmer = std::time(nullptr);                                                                                   \
        std::cerr << counter << " timmer On: " << __FILE__ << ":" << __LINE__ << std::endl;                            \
    }

#define timmerOff                                                                                                      \
    {                                                                                                                  \
        std::cerr << counter << " timmer Off" << __FILE__ << ":" << __LINE__ << std::endl;                             \
        std::cerr << "timmer " << counter << ": " << std::difftime(std::time(nullptr), timmer) << " seconds"           \
                  << std::endl;                                                                                        \
        timmer = 0;                                                                                                    \
    }

template <typename T> struct is_sequential_counter : std::false_type {};
template <typename T, typename Allocator> struct is_sequential_counter<std::vector<T, Allocator>> : std::true_type {};
template <typename T, typename Allocator> struct is_sequential_counter<std::list<T, Allocator>> : std::true_type {};

using string = std::string;

template <typename T> inline bool inRange(T value, T low, T high) { return value >= low && value <= high; }

template <typename T, typename... Args> inline bool inSet(T value, const Args &...Enum) {
    static_assert(sizeof...(Args) != 0, "At least provide one Enum value");

    return ((value == Enum) || ...);
}

template <typename Tx, typename Ty, typename Tuple, std::size_t... I>
Ty inSetAndMap_impl(Tx value, const Tuple &tuple, std::index_sequence<I...>) {
    Ty result = static_cast<Ty>(0);

    (void)((value == std::get<2 * I>(tuple) ? (result = std::get<2 * I + 1>(tuple), true) : true) && ...);

    return result;
}

template <typename Tx, typename Ty, typename... Args>
inline Ty inSetAndMap(Tx value, Args &&...Enums) { /* Tx, Ty, Tx, Ty... */
    static_assert(sizeof...(Args) != 0 && sizeof...(Args) % 2 == 0, "Must provide even number of arguments");

    auto tuple = std::make_tuple(std::forward<Args>(Enums)...);
    constexpr std::size_t N = sizeof...(Args);

    return inSetAndMap_impl<Tx, Ty>(value, tuple, std::make_index_sequence<N / 2>{});
}

template <typename Tuple, std::size_t... I> bool all_equal_pairs_impl(const Tuple &tuple, std::index_sequence<I...>) {
    return ((std::get<2 * I>(tuple) == std::get<2 * I + 1>(tuple)) && ...);
}

template <typename... Args> bool all_equal_pairs(Args &&...args) {
    static_assert(sizeof...(Args) != 0 && sizeof...(Args) % 2 == 0, "Must provide even number of arguments");
    auto tuple = std::make_tuple(std::forward<Args>(args)...);
    constexpr std::size_t N = sizeof...(Args);
    return all_equal_pairs_impl(tuple, std::make_index_sequence<N / 2>{});
}

#endif
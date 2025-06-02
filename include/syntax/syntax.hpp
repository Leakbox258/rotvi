#pragma once
#ifndef ROTVI_SYNTAX_HPP
#define ROTVI_SYNTAX_HPP

#include <color.hpp>
#include <list>
#include <regex>
#include <type_traits>
#include <vector>

namespace syntax {

enum fileType {
    UNKNOWN,
    CXX
    ///@todo add more...
};

template <typename T> inline std::vector<const char *> tokenlizeImpl(T &&raw_text) {

    const char *token_flow_begin;
    std::vector<const char *> token_list{};

    if constexpr (std::is_same_v<string, std::remove_const_t<std::remove_reference_t<T>>>) {
        token_flow_begin = raw_text.c_str();
    } else if constexpr (std::is_same_v<char *, std::remove_const_t<std::remove_reference_t<T>>>) {
        token_flow_begin = raw_text;
    } else {
        static_assert(false);
    }

    token_list.emplace_back(token_flow_begin);

    char *current_pos = token_flow_begin;
    char *comment_pos = nullptr;

    while (*current_pos) {

        ///@brief Handle comments
        if (*current_pos == '/' && *(current_pos + 1) == '/') {
            if (comment_pos == nullptr) {
                comment_pos = current_pos;
            }
            // Skip to the end of the line
            while (*current_pos && *current_pos != '\n') {
                current_pos += 1;
            }
            token_list.emplace_back(comment_pos);
            comment_pos = nullptr; // Reset for next comment
            continue;
        } else if (*current_pos == '/' && *(current_pos + 1) == '*') {
            if (comment_pos == nullptr) {
                comment_pos = current_pos;
            }
            // Skip until we find the closing */
            while (*current_pos && !(*current_pos == '*' && *(current_pos + 1) == '/')) {
                current_pos += 1;
            }

            if (*current_pos) {   // If we found the closing */
                current_pos += 2; // Skip past */
                token_list.emplace_back(comment_pos);
                comment_pos = nullptr; // Reset for next comment
                continue;
            }
        }

        if (inSet(*current_pos, '\t', '\r', '\n', ' ')) {
            current_pos += 1;
            token_list.emplace_back(current_pos);
        } else if (inSet(*current_pos, '(', ')', '[', ']', '{', '}', '+', '-', '*', '!', '@', '#', '$', '%', '^', '&',
                         '=', '`', '~', '\\', ':', ';', '\'', '\"', ',', '<', '.', '>', '/', '?')) {
            token_list.emplace_back(current_pos);
            current_pos += 1;
        } else {
            current_pos += 1;
        }
    }

    return token_list;
}

template <typename T> struct is_sequential : std::false_type {};
template <typename T, typename Allocator> struct is_sequential<std::list<T, Allocator>> : std::true_type {};
template <typename T, typename Allocator> struct is_sequential<std::vector<T, Allocator>> : std::true_type {};

template <typename T> inline std::vector<const char *> tokenlize(T &&raw_text) {
    static_assert(is_sequential<std::remove_const_t<std::remove_reference_t<T>>>::value);

    std::list<const char *> token_flow;

    for (auto raw_text_line : raw_text) {
        token_flow.splice(token_flow.end(), tokenlizeImpl(raw_text_line));
    }

    return std::vector<const char *>(token_flow);
}

template <fileType type> inline std::pair<api::color, api::color> colorMatchImpl(const char *token_Cstr);

template <fileType type, typename T> inline std::pair<api::color, api::color> colorMatch(T &&token) {
    const char *token_Cstr;

    if constexpr (std::is_same_v<string, std::remove_const_t<std::remove_reference_t<T>>>) {
        token_Cstr = token.c_str();
    } else if constexpr (std::is_same_v<char *, std::remove_const_t<std::remove_reference_t<T>>>) {
        token_Cstr = token;
    } else {
        static_assert(false);
    }

    return colorMatchImpl<type>(token_Cstr);
}

}; // namespace syntax

#endif
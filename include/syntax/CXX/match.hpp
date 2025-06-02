#pragma once
#ifndef ROTVI_SYNTAX_CXX_MATCH_HPP
#define ROTVI_SYNTAX_CXX_MATCH_HPP

#include <keywords.hpp>
#include <syntax.hpp>

namespace syntax {
template <> inline std::pair<api::color, api::color> colorMatchImpl<syntax::CXX>(const char *token_Cstr) {

    if (token_Cstr[0] == '#') {
        return {api::color::magenta, api::color::default_color};
    }

    for (const char *kw : CXX_syntax::keywords) {
        if (strcmp(token_Cstr, kw) == 0) {
            return {api::color::blue, api::color::default_color};
        }
    }

    if (std::regex_match(token_Cstr, CXX_syntax::numeric_rx)) {
        return {api::color::yellow, api::color::default_color};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::string_lit_rx)) {
        return {api::color::green, api::color::default_color};
    }
    if (std::regex_match(token_Cstr, CXX_syntax::char_lit_rx)) {
        return {api::color::green, api::color::default_color};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::comments_rx)) {
        return {api::color::cyan, api::color::default_color};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::identifier_rx)) {
        return {api::color::white, api::color::default_color}; // 更亮的白色
    }

    return {api::color::default_color, api::color::default_color};
}
}; // namespace syntax

#endif
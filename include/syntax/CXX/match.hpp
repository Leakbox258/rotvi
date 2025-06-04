#pragma once
#ifndef ROTVI_SYNTAX_CXX_MATCH_HPP
#define ROTVI_SYNTAX_CXX_MATCH_HPP

#include <keywords.hpp>
#include <syntax/syntax.hpp>

namespace syntax {
template <>
inline std::pair<const api::ColorAttr &, const api::ColorAttr &> colorMatchImpl<syntax::CXX>(const char *token_Cstr) {

    if (token_Cstr[0] == '#') {
        static const auto colorOn = api::ColorAttrOn<api::color::magenta, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::magenta, api::color::default_color>();
        return {colorOn, colorOff};
    }

    for (const char *kw : CXX_syntax::keywords) {
        if (strcmp(token_Cstr, kw) == 0) {
            static const auto colorOn = api::ColorAttrOn<api::color::blue, api::color::default_color>();
            static const auto colorOff = api::ColorAttrOff<api::color::blue, api::color::default_color>();
            return {colorOn, colorOff};
        }
    }

    if (std::regex_match(token_Cstr, CXX_syntax::numeric_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::yellow, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::yellow, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::string_lit_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::green, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::green, api::color::default_color>();
        return {colorOn, colorOff};
    }
    if (std::regex_match(token_Cstr, CXX_syntax::char_lit_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::green, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::green, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::comments_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::cyan, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::cyan, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, CXX_syntax::identifier_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::white, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::white, api::color::default_color>();
        return {colorOn, colorOff}; // 更亮的白色
    }

    static const auto colorOn = api::ColorAttrOn<api::color::default_color, api::color::default_color>();
    static const auto colorOff = api::ColorAttrOff<api::color::default_color, api::color::default_color>();
    return {colorOn, colorOff};
}
}; // namespace syntax

#endif
#pragma once
#ifndef ROTVI_SYNTAX_CXX_MATCH_HPP
#define ROTVI_SYNTAX_CXX_MATCH_HPP

#include <keywords.hpp>
#include <syntax/syntax.hpp>

namespace syntax {
template <>
inline std::pair<const api::ColorAttr &, const api::ColorAttr &> colorMatchImpl<syntax::CXX>(const char *token_Cstr,
                                                                                             int len) {

    if (token_Cstr[0] == '#') {
        static const auto colorOn = api::ColorAttrOn<api::color::Darkplus_Orchid, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::Darkplus_Orchid, api::color::default_color>();
        return {colorOn, colorOff};
    }

    for (const char *kw : CXX_syntax::keywords) {
        if (strncmp(token_Cstr, kw, strlen(kw)) == 0) {
            static const auto colorOn = api::ColorAttrOn<api::color::blue, api::color::default_color>();
            static const auto colorOff = api::ColorAttrOff<api::color::blue, api::color::default_color>();
            return {colorOn, colorOff};
        }
    }

    for (const char *kw_fc : CXX_syntax::keywords_flowcontrol) {
        if (strncmp(token_Cstr, kw_fc, strlen(kw_fc)) == 0) {
            static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_HotPink, api::color::default_color>();
            static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_HotPink, api::color::default_color>();
            return {colorOn, colorOff};
        }
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::string_lit_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::char_lit_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::numeric_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_PaleGreen, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_PaleGreen, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::comments_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_SlateBlue, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_SlateBlue, api::color::default_color>();
        return {colorOn, colorOff};
    }

    ///@todo 函数名怎么不算是一种标识符
    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::identifier_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::yellow, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::yellow, api::color::default_color>();
        return {colorOn, colorOff};
    }

    static const auto colorOn = api::ColorAttrOn<api::color::default_color, api::color::default_color>();
    static const auto colorOff = api::ColorAttrOff<api::color::default_color, api::color::default_color>();
    return {colorOn, colorOff};
}
}; // namespace syntax

#endif
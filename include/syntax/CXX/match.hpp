#pragma once
#ifndef ROTVI_SYNTAX_CXX_MATCH_HPP
#define ROTVI_SYNTAX_CXX_MATCH_HPP

#include <keywords.hpp>
#include <syntax/syntax.hpp>

namespace syntax {
template <>
inline std::pair<const api::ColorAttr &, const api::ColorAttr &>
colorMatchImpl<syntax::CXX>(const char *token_Cstr, int len, token::attr attr) {

    if (token_Cstr[0] == '#') {
        static const auto colorOn = api::ColorAttrOn<api::color::Darkplus_Orchid, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::Darkplus_Orchid, api::color::default_color>();
        return {colorOn, colorOff};
    }

    for (const char *kw : CXX_syntax::keywords) {
        if (strncmp(token_Cstr, kw, strlen(kw)) == 0) {
            static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_DeepBlue, api::color::default_color>();
            static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_DeepBlue, api::color::default_color>();
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

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::string_lit_rx) || attr == token::attr::String) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::char_lit_rx) || attr == token::attr::String) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LemonChiffon, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::numeric_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_PaleGreen, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_PaleGreen, api::color::default_color>();
        return {colorOn, colorOff};
    }

    ///@note in one line comment cant be recognized by regex
    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::comments_rx) || attr == token::attr::Comment) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_SlateBlue, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_SlateBlue, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (attr == token::attr::Function) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LightYellow, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LightYellow, api::color::default_color>();
        return {colorOn, colorOff};
    }

    if (std::regex_match(token_Cstr, token_Cstr + len, CXX_syntax::identifier_rx)) {
        static const auto colorOn = api::ColorAttrOn<api::color::DarkPlus_LightGray, api::color::default_color>();
        static const auto colorOff = api::ColorAttrOff<api::color::DarkPlus_LightGray, api::color::default_color>();
        return {colorOn, colorOff};
    }

    static const auto colorOn = api::ColorAttrOn<api::color::default_color, api::color::default_color>();
    static const auto colorOff = api::ColorAttrOff<api::color::default_color, api::color::default_color>();
    return {colorOn, colorOff};
}
}; // namespace syntax

#endif
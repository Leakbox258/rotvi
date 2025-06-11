#pragma once
#ifndef ROTVI_COLOR_HPP
#define ROTVI_COLOR_HPP

#include <algorithm>
#include <api.hpp>
#include <map>
#include <set>
#include <tools.hpp>

namespace api {

///@note attr get from ncurser
inline bool has_color = false;
inline bool use_default = false;

enum color : u_int8_t {
    default_color = 0XFF,
    black = COLOR_BLACK,
    yellow = COLOR_YELLOW,
    red = COLOR_RED,
    green = COLOR_GREEN,
    blue = COLOR_BLUE,
    cyan = COLOR_CYAN,
    magenta = COLOR_MAGENTA,
    white = COLOR_WHITE,
    DarkPlus_HotPink,
    DarkPlus_LemonChiffon,
    DarkPlus_PaleGreen,
    DarkPlus_LightGray, // ide is different from text editor
    DarkPlus_Cyan,
    DarkPlus_Aquamarine,
    DarkPlus_SlateBlue,
    Darkplus_Orchid,
    DarkPlus_LightYellow,
    DarkPlus_DeepBlue,
    counter,
};

inline const std::map<color, std::array<u_int8_t, 3>> RGBList = // NOLINT
    {{DarkPlus_HotPink, {0XC5, 0X86, 0XC0}},     {DarkPlus_LemonChiffon, {0XCE, 0X91, 0X78}},
     {DarkPlus_PaleGreen, {0XB5, 0XCE, 0XA8}},   {DarkPlus_LightGray, {0X9C, 0XDC, 0XFE}},
     {DarkPlus_Cyan, {0X4E, 0XC9, 0XB0}},        {DarkPlus_Aquamarine, {78, 201, 176}},
     {DarkPlus_SlateBlue, {0X6A, 0X99, 0X55}},   {Darkplus_Orchid, {0XC5, 0X86, 0XC0}},
     {DarkPlus_LightYellow, {0XDC, 0XDC, 0XAA}}, {DarkPlus_DeepBlue, {0X56, 0X9C, 0XD6}}};

inline constexpr u_int16_t bitPack(color Front, color Back) {
    u_int16_t pack = 0x0;
    pack |= Front << 8;
    pack |= Back;
    return pack;
}

inline constexpr std::pair<int8_t, int8_t> bitUnPack(u_int16_t pack) {
    std::pair<int8_t, int8_t> unpack;

    (pack & 0XFF00) == 0XFF00 ? unpack.first = static_cast<int8_t>(-1) : unpack.first = pack >> 8;
    (pack & 0XFF) == 0XFF ? unpack.second = static_cast<int8_t>(-1) : unpack.second = pack & 0xFF;

    return unpack;
}

///@brief static color pair register
namespace detail {

inline std::set<u_int16_t> &getColorPacks() {
    static std::set<u_int16_t> ColorPacks;
    return ColorPacks;
}

inline std::set<color> &getColors() {
    static std::set<color> Colors;
    return Colors;
}

inline std::map<u_int16_t, u_int8_t> &getColorPairIdx() {
    static std::map<u_int16_t, u_int8_t> ColorPairIdx;
    return ColorPairIdx;
}

inline u_int8_t pairCnter = 0;

inline void RegisterHelper(color Front, color Back) noexcept {
    getColorPacks().emplace(bitPack(Front, Back)); // NOLINT

    if (Front != 0xFF && Front > 7) {
        getColors().emplace(Front);
    }
    if (Back != 0XFF && Back > 7) {
        getColors().emplace(Back);
    }

    getColorPairIdx()[bitPack(Front, Back)] = pairCnter++;
    ///@todo add cnter range check here
}

template <color Front, color Back> struct CTRegistrar {
    inline static bool inUse = (RegisterHelper(Front, Back), true);
};

}; // namespace detail

inline void Init_pair(u_int16_t pack) {
    auto unpack = bitUnPack(pack);

    //  (zext, sext, sext)
    init_extended_pair(detail::getColorPairIdx().at(pack), static_cast<int>(unpack.first),
                       static_cast<int>(unpack.second));
}

inline void Init_color(color color) {
    constexpr u_int8_t R = 0;
    constexpr u_int8_t G = 1;
    constexpr u_int8_t B = 2;
    const auto &rgb = RGBList.at(color);

    constexpr auto ncurser_rgb_ext = [](u_int8_t hex) -> int {
        return static_cast<int>(hex) * 1000 / 255; // NOLINT
    };

    init_extended_color(static_cast<unsigned>(color),
                        ncurser_rgb_ext(rgb[R]), // NOLINT
                        ncurser_rgb_ext(rgb[G]), // NOLINT
                        ncurser_rgb_ext(rgb[B]));
}

struct ColorAttr {
    virtual void operator()() const = 0;
};

template <color Front, color Back> struct ColorAttrOn : public ColorAttr {
    void operator()() const override {
        constexpr auto pack = bitPack(Front, Back);

        [[maybe_unused]] static detail::CTRegistrar<Front, Back> uniqueRegi;
        (void)uniqueRegi.inUse;

        attron(COLOR_PAIR(api::detail::getColorPairIdx().at(pack)));
    }
};

template <color Front, color Back> struct ColorAttrOff : public ColorAttr {
    void operator()() const override {
        constexpr auto pack = bitPack(Front, Back);
        /// @todo add static object check here

        attroff(COLOR_PAIR(api::detail::getColorPairIdx().at(pack)));
    }
};
}; // namespace api

using color = api::color;

#endif
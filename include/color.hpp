#pragma once
#ifndef ROTVI_COLOR_HPP
#define ROTVI_COLOR_HPP

#include <algorithm>
#include <api.hpp>
#include <set>
#include <tools.hpp>

namespace api {

///@note attr get from ncurser
inline bool has_color = false;
inline bool use_default = false;

enum color : int8_t {
    default_color = -1,
    black = COLOR_BLACK,
    yellow = COLOR_YELLOW,
    red = COLOR_RED,
    green = COLOR_GREEN,
    blue = COLOR_BLUE,
    cyan = COLOR_CYAN,
    magenta = COLOR_MAGENTA,
    white = COLOR_WHITE,
    counter = 8
};

inline constexpr u_int8_t bitPack(color Front, color Back) {
    u_int8_t pack = 0x0;
    pack = Front != color::default_color ? pack | (Front << 4) : pack | 0X80; // avoid unuse check
    pack = Back != color::default_color ? pack | Back : pack | 0x8;           // avoid unuse check
    return pack;
}

inline constexpr std::pair<short, short> bitUnPack(u_int8_t pack) {
    std::pair<short, short> unpack;

    pack & 0X80 ? unpack.first = static_cast<short>(-1) : unpack.first = static_cast<short>((pack >> 4) & 0XF);
    pack & 0X8 ? unpack.second = static_cast<short>(-1) : unpack.second = static_cast<short>(pack & 0XF);

    return unpack;
}

inline void Init_pair(u_int8_t pack) {
    auto unpack = bitUnPack(pack);
    init_pair(static_cast<short>(pack), unpack.first, unpack.second); // NOLINT
}

///@brief static color pair register
namespace detail {

inline std::set<u_int8_t> &getColorPacks() {
    static std::set<u_int8_t> ColorPacks;
    return ColorPacks;
}

inline void RegisterHelper(color Front, color Back) noexcept {
    getColorPacks().emplace(bitPack(Front, Back)); // NOLINT
}

template <color Front, color Back> struct CTRegistrar {
    inline static bool inUse = (RegisterHelper(Front, Back), true);
};

}; // namespace detail

struct ColorAttr {
    virtual void operator()() const = 0;
};

template <color Front, color Back> struct ColorAttrOn : public ColorAttr {
    void operator()() const override {
        constexpr auto pack = bitPack(Front, Back);

        [[maybe_unused]] static detail::CTRegistrar<Front, Back> uniqueRegi;
        (void)uniqueRegi.inUse;

        attron(COLOR_PAIR(static_cast<short>(pack)));
    }
};

template <color Front, color Back> struct ColorAttrOff : public ColorAttr {
    void operator()() const override {
        constexpr auto pack = bitPack(Front, Back);
        /// @todo add static object check here

        attroff(COLOR_PAIR(static_cast<short>(pack)));
    }
};

}; // namespace api

using color = api::color;

#endif
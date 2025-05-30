#pragma once
#ifndef ROTVI_API_HPP
#define ROTVI_API_HPP

#if __has_include(<ncurses.h>)
#include <ncurses.h>
#elif __has_include(<curses.h>)
#include <curses.h>
#else
#error "ncurses.h or curses.h not found. Please install ncurses development library."
#endif

#include <cctype>
#include <stdlib.h>
#include <utility>

namespace api {

const int Key_down = 0402;
const int Key_up = 0403;
const int Key_left = 0404;
const int Key_right = 0405;
const int Key_home = 0406;
const int Key_backspace = 0407;
const int Key_end = 0550;
const int Key_enter = 0527;

const int Key_dl = 0510;
const int Key_il = 0511;
const int Key_dc = 0512;
const int Key_ic = 0513;

inline void Initscr() {
    initscr(); // NOLINT
}

inline void Raw() {
    raw(); // NOLINT
}

inline void Keypad(WINDOW *scr = stdscr, bool enable = true) {
    keypad(scr, enable); // Enable or disable keypad input
}

inline void Noecho() {
    noecho(); // NOLINT
}

inline void Beep() {
    beep(); // NOLINT
}

inline int Getch() {
    return getch(); // NOLINT
}

inline void Getmaxyx(WINDOW *scr, int &rows, int &cols) {
    rows = getmaxy(scr), cols = getmaxx(scr); // NOLINT
}

inline int Isprint(int ch) {
    return isprint(ch); // Check if character is printable
}

inline void Erase() {
    erase(); // Clear the ncurses virtual screen
}

inline void Attron() {
    attron(A_REVERSE); // Highlight
}

inline void Attroff() {
    attroff(A_REVERSE); // NOLINT
}

template <typename... Args> inline void Mvprintw(int y, int x, const char *chs, Args &&...args) {
    mvprintw(y, x, chs, std::forward<Args>(args)...); // pwnable
}

inline int Move(int y, int x) {
    return move(y, x); // NOLINT
}

inline void Mvaddch(int y, int x, char ch) {
    mvaddch(y, x, ch); // Clear the status line
}

inline void Refresh() {
    refresh(); // NOLINT
}

inline void Endwin() {
    endwin(); // NOLINT
}

inline void ForceQuit(int code) {
    exit(code); // NOLINT
}

}; // namespace api

#endif
#include <color.hpp>
#include <cursor.hpp>

void api_init() {
    api::Initscr();            // Start curses mode
    api::Raw();                // Line buffering disabled
    api::Keypad(stdscr, TRUE); // Enable F1, arrows, etc.
    api::Noecho();             // Don't echo() input
    // Optional: nonl(); // Don't translate newline char to CR/LF on output

    if (api::Has_colors() && api::Can_change_color()) {
        api::has_color = true;
        api::Start_color();

        if (api::Use_default_colors() == OK) {
            api::use_default = true;
        }

        for (auto color : api::detail::getColors()) {
            api::Init_color(color);
        }

        for (auto idx : api::detail::getColorPacks()) {
            api::Init_pair(idx);
        }
    }

    api::Getmaxyx(stdscr, term_rows, term_cols); // Get terminal dimensions
}
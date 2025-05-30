#include <algorithm> // For std::min/max
#include <cursor.hpp>

cursorExtern;
timeExtern;

int main(int argc, char *argv[]) {
    // ncurses initialization
    api::Initscr();            // Start curses mode
    api::Raw();                // Line buffering disabled
    api::Keypad(stdscr, TRUE); // Enable F1, arrows, etc.
    api::Noecho();             // Don't echo() input
    // Optional: nonl(); // Don't translate newline char to CR/LF on output

    api::Getmaxyx(stdscr, term_rows, term_cols); // Get terminal dimensions

    cursor mainCursor;
    if (argc > 1) {
        mainCursor = fileWriteIn(argv[1]);
    } else {
        mainCursor = fileWriteIn();
    }

    mainCursor.setToEOF(); // Initial clamp

    int ch;
    while (true) {
        mainCursor.setToEOF();
        mainCursor.redrawScreen();
        ch = api::Getch();

        mainCursor.pressHandler(ch);
    }

    // ncurses cleanup
    api::Endwin();
    return 0;
}
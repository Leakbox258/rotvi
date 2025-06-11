#include <algorithm> // For std::min/max
#include <cursor.hpp>

int main(int argc, char *argv[]) {

    api_init();

    std::unique_ptr<cursor> mainCursor;
    if (argc > 1) {
        mainCursor = std::move(fileWriteIn(argv[1]));
    } else {
        mainCursor = std::move(fileWriteIn());
    }

    mainCursor->clamp(); // Initial clamp

    int ch;
    while (true) {
        mainCursor->clamp();
        mainCursor->redrawScreen();
        ch = api::Getch();

        mainCursor->pressHandler(ch);
    }

    // ncurses cleanup
    api::Endwin();
    return 0;
}
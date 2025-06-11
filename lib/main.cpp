#include <algorithm> // For std::min/max
#include <cursor.hpp>

int main(int argc, char *argv[]) {

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        printf(SOFTWARE_INFO);
        printf("Usage: rotvi [filename] "
               "    If no filename is provided, it will open a new file.\n");
        return 0;
    }

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
#include <cursor.hpp>

template <> void cursor::pressHandler<Mode::NORMAL>(int ch) {

    renewStatus("-- NORMAL --");

    switch (ch) { // NOLINT
    case api::Key_left:
    case 'h':
        mvLeft();
        break;
    case api::Key_right:
    case 'l':
        mvRight();
        break;
    case api::Key_up:
    case 'k':
        mvUp();
        break;
    case api::Key_down:
    case 'j':
        mvDown();
        break;
    case api::Key_home:
    case '0':
        mvHome();
        break;
    case api::Key_end:
    case '$':
        mvEnd();
        break;
    case 'i':
        chmode(Mode::INSERT);
        break;
    case 'a': {
        if (!lineCur().empty() || _col_ > 0) { // if not empty line, or not at col 0 of empty line
            _col_ = std::min(static_cast<int>(lineCur().length()), _col_ + 1);
        }

        chmode(Mode::INSERT);
        break;
    }
    case 'o': // Open line below
        _lines_buf_.insert(_lines_buf_.begin() + _row_ + 1, "");
        ++_row_;
        _col_ = 0;
        chmode(Mode::INSERT);
        break;
    case 'O': // Open line above
        _lines_buf_.insert(_lines_buf_.begin() + _row_, "");
        // _cursor_row_ remains same, new line inserted at _row_
        _col_ = 0;
        chmode(Mode::INSERT);
        break;
    case 'x': // Delete character under cursor
        if (lineColNr(_row_) && _col_ < static_cast<int>(lineColNr(_row_))) {
            lineCur().erase(_col_, 1);
            modified = true;
        }
        break;
    case 'd':              // Potentially start 'dd'
        ch = api::Getch(); // Wait for next char
        if (ch == 'd') {   // 'dd' - delete line
            if (!lineEmpty()) {
                _lines_buf_.erase(_lines_buf_.begin() + _row_);
                if (lineEmpty()) {
                    lineAppend("");
                    // Ensure buffer is not completely empty
                    if (_row_ >= static_cast<int>(lineNr())) {
                        // if deleted last line
                        _row_ = static_cast<int>(lineNr()) - 1;
                    }
                }
            } else {
                api::Beep();
            }
            break;
        case ':':
            chmode(Mode::COMMAND_LINE);
            _cmd_buf_.clear();
            renewStatus(":");
            _col_ = 0;
            break;
            // Add G to go to last line, gg to go to first line
        case 'G':
            _row_ = static_cast<int>(lineNr()) - 1;
            clamp(); // ensure col is valid
            break;
            // No 'gg' yet
        }
    }
}

template <> void cursor::pressHandler<Mode::INSERT>(int ch) {
    switch (ch) {
    case api::Key_down:
        mvDown();
        break;
    case api::Key_up:
        mvUp();
        break;
    case api::Key_left:
        mvLeft();
        break;
    case api::Key_right:
        mvRight();
        break;
    case api::Key_home:
        mvHome();
        break;
    case api::Key_end:
        mvEnd();
        break;
    case api::Key_backspace: // Typically 127 or 263, or '\b'
    case 127:                // ASCII DEL
    case api::Ctrl('h'):     // ASCII BS (often map to the same in raw mode by terminals)
        if (_col_ > 0) {
            lineCur().erase(_col_ - 1, 1);
            --_col_;
            modified = true;
        } else if (_row_ > 0) {
            // Backspace at beginning of line
            // Join with previous line
            string current_line_content = lineCur();

            _lines_buf_.erase(_lines_buf_.begin() + _row_);
            --_row_;
            _col_ = static_cast<int>(lineCur().length());

            lineCur() += current_line_content;
            modified = true;
        }
        break;
    case api::Key_dc:
        if (!lineCur().empty() && _col_ < lineCur().length()) {
            lineCur().erase(_col_, 1);
            modified = true;
        }
        break;
    case api::Key_escape: // Escape key
        mvLeft();
        chmode(Mode::NORMAL);
        break;
    case api::Key_enter:
    case '\n':
    case '\r': {
        string current_line_suffix = lineCur().substr(_col_);
        lineCur().erase(_col_); // Remove suffix from current line
        _lines_buf_.insert(_lines_buf_.begin() + _row_ + 1,
                           current_line_suffix); // Insert suffix as new line
        ++_row_;
        _col_ = 0;

        modified = true;
    } break;
    default:
        // Insert normal characters
        if (api::Isprint(ch) || ch == '\t') {
            lineCur().insert(_col_, 1, static_cast<char>(ch));
            ++_col_;

            modified = true;
        }
        break;
    }
}

template <> void cursor::pressHandler<Mode::COMMAND_LINE>(int ch) {
    switch (ch) {
    case api::Key_left:
        cmdmvLeft();
        break;
    case api::Key_right:
        cmdmvRight();
        break;
    case api::Ctrl('h'):
    case api::Key_backspace:
    case 127:
        if (_col_ > 0) {
            _cmd_buf_.erase(_col_ - 1);
            --_col_;
        }
        break;
    case api::Key_escape: // Escape key
        chmode(Mode::NORMAL);
        _cmd_buf_.clear();
        break;
    case api::Key_enter:
    case '\n':
    case '\r':
        cmdlineProcessor();
        chmode(Mode::NORMAL);
        break;
    default:
        if (api::Isprint(ch)) {
            _cmd_buf_.insert(_col_, 1, static_cast<char>(ch));
            ++_col_;
        }
        break;
    }
}

void cursor::pressHandler(int ch) {

    if (mode() == Mode::NORMAL) {
        pressHandler<Mode::NORMAL>(ch);
    } else if (mode() == Mode::INSERT) {
        pressHandler<Mode::INSERT>(ch);
    } else if (mode() == Mode::COMMAND_LINE) {
        pressHandler<Mode::COMMAND_LINE>(ch);
    }
}
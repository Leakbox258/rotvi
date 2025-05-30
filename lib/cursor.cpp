#include <cursor.hpp>
///@todo 支持分屏

// Terminal dimensions
int term_rows, term_cols;

config cursorDefaultCfg = {.mode = Mode::NORMAL, .autoChangLine = false};

int anonymouseNr = 0;

std::map<string, cursor> openedCursors;

void cursor::fileWriteBack(const string &filename) {

    std::ofstream stream(filename);

    if (stream) {
        for (const auto &line : this->_lines_buf_) {
            stream << line << std::endl; // write back
        }
        stream.close();
        this->renewStatus("\"" + filename + "\" saved.");
        this->renewFileName(filename);
    } else {
        this->renewStatus("Error: Could not open file for writing: " + filename);
    }
}

void cursor::setToEOF() {
    if (lineEmpty()) {
        lineAppend("");
    }

    _row_ = std::max(0, std::min(_row_, static_cast<int>(_lines_buf_.size()) - 1));

    _col_ = std::max(0, std::min(_col_, static_cast<int>(_lines_buf_[_row_].length())));

    if (mode() == Mode::NORMAL && !_lines_buf_[_row_].empty() &&
        _col_ == static_cast<int>(_lines_buf_[_row_].length()) && _col_ > 0) {
        --_col_; // fix
    }

    mode() == Mode::NORMAL &&_lines_buf_[_row_].empty() ? void(_col_ = 0) : nop;

    _row_ < _win_top_row_ ? void(_win_top_row_ = _row_) : nop;

    _row_ >= _win_top_row_ + term_rows - 1 ? void(_win_top_row_ = _row_ - (term_rows - 1) + 1)
                                           : nop; // -1 for status bar

    _col_ < _win_left_col_ ? void(_win_left_col_ = _col_) : nop;

    _col_ >= _win_left_col_ + term_cols ? void(_win_left_col_ = _col_ - term_cols + 1) : nop;
}

void cursor::redrawScreen() {
    api::Erase();

    for (int i = 0; i < term_rows - 1; ++i) { // -1 for status bar
        int doc_line_idx = _win_top_row_ + i;
        if (doc_line_idx < static_cast<int>(_lines_buf_.size())) {
            const string &line_to_draw = _lines_buf_[doc_line_idx];
            // Handle horizontal scrolling
            int len_to_draw = static_cast<int>(line_to_draw.size()) - _win_left_col_;
            if (len_to_draw > 0) {
                api::Mvprintw(i, 0, "%.*s", term_cols,
                              line_to_draw.substr(_win_left_col_, line_to_draw.length()).c_str());
            }
        } else {
            mvprintw(i, 0, "~");
        }
    }

    api::Attron();

    string mode_str;
    switch (_config_.mode) {
    case Mode::NORMAL:
        mode_str = "-- NORMAL --";
        break;
    case Mode::INSERT:
        mode_str = "-- INSERT --";
        break;
    case Mode::COMMAND_LINE:
        mode_str = ":";
        break;
    }

    string left_status = mode_str + " " + _file_name_;
    string right_status = "Ln " + std::to_string(_row_ + 1) + ", Col " + std::to_string(_col_ + 1);

    api::Mvprintw(term_rows - 1, 0, "%s", left_status.c_str());
    int right_status_len = static_cast<int>(right_status.length());
    if (term_cols > static_cast<int>(left_status.length()) + right_status_len) {
        api::Mvprintw(term_rows - 1, term_cols - right_status_len - 1, "%s", right_status.c_str());
    }

    // Clear rest of status line
    for (int j = static_cast<int>(left_status.length()); j < term_cols - right_status_len - 1; ++j) {
        api::Mvaddch(term_rows - 1, j, ' ');
    }

    if (_config_.mode == Mode::COMMAND_LINE) {
        api::Mvprintw(term_rows - 1, 0, ":%s", _cmd_buf_.c_str());
    }

    api::Attroff();

    // Display status message if any, then clear it
    if (!_status_msg_.empty() && _config_.mode != Mode::COMMAND_LINE) {
        api::Attron();
        api::Mvprintw(term_rows - 1, (term_cols - static_cast<int>(_status_msg_.length())) / 2, "%s",
                      _status_msg_.c_str());
        api::Attroff();
        _status_msg_.clear(); // Show once
    }

    // Position the ncurses cursor
    if (_config_.mode == Mode::COMMAND_LINE) {
        api::Move(term_rows - 1, 1 + static_cast<int>(_cmd_buf_.length()));
    } else {
        api::Move(_row_ - _win_top_row_, _col_ - _win_left_col_);
    }

    api::Refresh(); // Refresh the screen to show changes
}

void cursor::pressHandler(int ch) {

    if (mode() == Mode::NORMAL) {
        switch (ch) {
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
            break; // Go to end, normal mode style
        case 'i':
            chmode(Mode::INSERT);
            break;
        case 'a': {
            if (!_lines_buf_[_row_].empty() || _col_ > 0) { // if not empty line, or not at col 0 of empty line
                _col_ = std::min(static_cast<int>(_lines_buf_[_row_].length()), _col_ + 1);
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
            if (!lineColNr(_row_) && _col_ < static_cast<int>(lineColNr(_row_))) {
                _lines_buf_[_row_].erase(_col_, 1);
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
                    // Unrecognized 'd' command, beep or ungetch(ch)
                    api::Beep();
                }
                break;
            case ':':
                chmode(Mode::COMMAND_LINE);
                _cmd_buf_.clear();
                renewStatus(":");
                break;
                // Add G to go to last line, gg to go to first line
            case 'G':
                _row_ = static_cast<int>(lineNr()) - 1;
                setToEOF(); // ensure col is valid
                break;
                // No 'gg' yet, can be added with timeout or sequence detection
            }
        }
    } else if (mode() == Mode::INSERT) {
        switch (ch) {
        case api::Key_backspace: // Typically 127 or 263, or '\b'
        case 127:                // ASCII DEL
        case 8:                  // ASCII BS (often map to the same in raw mode by terminals)
            if (_col_ > 0) {
                _lines_buf_[_row_].erase(_col_ - 1, 1);
                --_col_;
            } else if (_row_ > 0) {
                // Backspace at beginning of line
                // Join with previous line
                string &current_line_content = _lines_buf_[_row_];

                _lines_buf_.erase(_lines_buf_.begin() + _row_);
                --_row_;
                _col_ = static_cast<int>(_lines_buf_[_row_].length());

                _lines_buf_[_row_] += current_line_content;
            }
            break;
        case 27: // Escape key
            chmode(Mode::NORMAL);

            // In vi, Esc in insert mode moves cursor one left if not at start of line
            mvLeft();
            break;
        case api::Key_enter: // ncurses defined, or 10 (\n), 13 (\r)
        case '\n':
        case '\r': {
            string current_line_suffix = _lines_buf_[_row_].substr(_col_);
            _lines_buf_[_row_].erase(_col_); // Remove suffix from current line
            _lines_buf_.insert(_lines_buf_.begin() + _row_ + 1,
                               current_line_suffix); // Insert suffix as new line
            _row_++;
            _col_ = 0;
        } break;
        default:
            // Insert normal characters
            if (api::Isprint(ch) || ch == '\t') {
                _lines_buf_[_row_].insert(_col_, 1, static_cast<char>(ch));
                _col_++;
            }
            break;
        }
    } else if (mode() == Mode::COMMAND_LINE) {
        switch (ch) {
        case KEY_BACKSPACE:
        case 127:
        case 8:
            if (!_cmd_buf_.empty()) {
                _cmd_buf_.pop_back();
            }
            break;
        case 27: // Escape key
            chmode(Mode::COMMAND_LINE);
            _cmd_buf_.clear();
            break;
        case KEY_ENTER:
        case '\n':
        case '\r':
            processCmd();
            chmode(Mode::NORMAL); // return to normal mode
            break;
        default:
            if (api::Isprint(ch)) {
                _cmd_buf_ += static_cast<char>(ch);
            }
            break;
        }
    }
}

void cursor::processCmd() {

    if (_cmd_buf_.empty()) {
        return;
    }

    if (_cmd_buf_ == "q") {
        ///@todo Add check for unsaved changes

        api::Endwin();
        api::ForceQuit(0);
    } else if (_cmd_buf_ == "q!") {
        api::Endwin();
        api::ForceQuit(0);
    } else if (_cmd_buf_.rfind("w ", 0) == 0) {
        string filename_to_save = _cmd_buf_.substr(2);

        if (filename_to_save.empty()) {
            filename_to_save = _file_name_;
        }

        if (filename_to_save == "untitled" || filename_to_save.empty()) {
            _status_msg_ = "E32: No file name";
        } else {
            fileWriteBack(filename_to_save);
        }
    } else if (_cmd_buf_ == "wq") {
        if (_file_name_ == "untitled" || _file_name_.empty()) {
            _status_msg_ = "E32: No file name for :wq. Use :wq <filename>";
        } else {
            fileWriteBack(_file_name_);
            api::Endwin();
            api::ForceQuit(0);
        }
    } else if (_cmd_buf_.rfind("wq ", 0) == 0) {
        string filename_to_save = _cmd_buf_.substr(3);
        fileWriteBack(filename_to_save);
        if (_status_msg_.find("Error") == string::npos) { // if no error during save
            api::Endwin();
            api::ForceQuit(0);
        }
    } else {
        _status_msg_ = "Not an editor command: " + _cmd_buf_;
    }

    _cmd_buf_.clear();
}
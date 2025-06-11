#include <cursor.hpp>

// Terminal dimensions

void cursor::fileWriteBack(const string &filename) {

    std::ofstream stream(filename);

    if (stream) {
        for (const auto &line : _lines_buf_) {
            stream << line << std::endl;
        }
        stream.close();
        renewStatus("\"" + filename + "\" saved.");
        renewFileName(filename);
    } else {
        renewStatus("Error: Could not open file for writing: " + filename);
    }
}

void cursor::clamp() {
    if (lineEmpty()) {
        lineAppend("");
    }

    if (mode() == Mode::COMMAND_LINE) {
        return;
    }

    _row_ = std::max(0, std::min(_row_, static_cast<int>(_lines_buf_.size()) - 1));

    auto &currentLint = lineCur();

    _col_ = std::max(0, std::min(_col_, static_cast<int>(currentLint.length())));

    if (mode() == Mode::NORMAL && _col_ > 0 && !currentLint.empty() &&
        _col_ == static_cast<int>(currentLint.length())) {
        --_col_;
    }

    if (mode() == Mode::NORMAL && currentLint.empty()) {
        _col_ = 0;
    }

    _row_ < _win_top_row_ ? void(_win_top_row_ = _row_) : nop;

    _row_ >= _win_top_row_ + term_rows - 1 ? void(_win_top_row_ = _row_ - (term_rows - 1) + 1)
                                           : nop; // -1 for status bar

    _col_ < _win_left_col_ ? void(_win_left_col_ = _col_) : nop;

    _col_ >= _win_left_col_ + term_cols ? void(_win_left_col_ = _col_ - term_cols + 1) : nop;
}

void cursor::redrawScreen() {
    api::Erase();

    printer(*this);

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

    if (mode() != Mode::COMMAND_LINE) {
        api::Mvprintw(term_rows - 1, 0, "%s", left_status.c_str());
    } else {
        api::Mvprintw(term_rows - 1, 0, string(left_status.length(), ' ').c_str());
    }

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

    // Display status message if any, then clear it
    if (!_status_msg_.empty() && _config_.mode != Mode::COMMAND_LINE) {
        api::Mvprintw(term_rows - 1, (term_cols - static_cast<int>(_status_msg_.length())) / 2, "%s",
                      _status_msg_.c_str());
        _status_msg_.clear(); // Show once
    }

    // Position the ncurses cursor
    if (_config_.mode == Mode::COMMAND_LINE) {
        api::Move(term_rows - 1, _col_ + 1);
    } else {
        api::Move(_row_ - _win_top_row_, calScrCol(*this) - _win_left_col_);
    }

    api::Refresh();
}

///@brief calculate where to display the cursor with tabs exsist
int calScrCol(const cursor &cursor_v) {
    const std::string &line = cursor_v.lineCur();

    int current_screen_col = 0;
    int line_actual_length = static_cast<int>(line.length());

    for (int i = 0; i < cursor_v._col_; ++i) {
        if (i < line_actual_length) {
            char ch = line[i];
            if (ch == '\t') {
                current_screen_col +=
                    static_cast<int>(cursor_v._config_.indent - (current_screen_col % cursor_v._config_.indent));
            } else {
                current_screen_col += 1;
            }
        } else {
            current_screen_col += 1;
        }
    }
    return current_screen_col;
}

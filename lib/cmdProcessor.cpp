#include <cursor.hpp>

void cursor::cmdlineProcessor() {

    if (_cmd_buf_.empty()) {
        return;
    }

    if (_cmd_buf_ == "q") { // NOLINT
        if (modified) {
            _status_msg_ = "E37: No write since last change (add ! to override)";
        } else {
            api::Endwin();
            api::ForceQuit(0);
        }
    } else if (_cmd_buf_ == "q!") {
        modified = false;

        api::Endwin();
        api::ForceQuit(0);
    } else if (_cmd_buf_.rfind("w ", 0) == 0) {
        modified = false;

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
            modified = false;

            fileWriteBack(_file_name_);
            api::Endwin();
            api::ForceQuit(0);
        }
    } else if (_cmd_buf_.rfind("wq ", 0) == 0) {
        modified = false;

        string filename_to_save = _cmd_buf_.substr(3);
        fileWriteBack(filename_to_save);
        if (_status_msg_.find("Error") == string::npos) { // if no error during save
            api::Endwin();
            api::ForceQuit(0);
        }
    } else if (_cmd_buf_ == "up") {
        int page_height = term_rows > 1 ? term_rows - 1 : 1;

        _row_ -= page_height;
        if (_row_ < 0) {
            _row_ = 0;
        }

        _win_top_row_ = _row_;
        if (_win_top_row_ < 0) {
            _win_top_row_ = 0;
        }

        if (_row_ < lineNr()) {
            _col_ = std::min(_col_, lineColNr(_row_));
        } else {
            _col_ = 0;
        }
    } else if (_cmd_buf_ == "down") {
        int page_height = term_rows > 1 ? term_rows - 1 : 1;
        int last_line_index = lineNr() > 0 ? lineNr() - 1 : 0;

        _row_ += page_height;

        if (_row_ > last_line_index) {
            _row_ = last_line_index;
        }

        _win_top_row_ = _row_;

        if (lineNr() > (unsigned)(term_rows - 1)) {
            int max_possible_top_row = lineNr() - (term_rows - 1);
            if (_win_top_row_ > max_possible_top_row) {
                _win_top_row_ = max_possible_top_row;
            }
        } else {

            _win_top_row_ = 0;
        }

        if (_row_ < lineNr()) {
            _col_ = std::min(_col_, lineColNr(_row_));
        } else {
            _col_ = 0;
        }
    } else {
        _status_msg_ = "Not an editor command: " + _cmd_buf_;
    }

    _cmd_buf_.clear();
}
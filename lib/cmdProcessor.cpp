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
    } else {
        _status_msg_ = "Not an editor command: " + _cmd_buf_;
    }

    _cmd_buf_.clear();
}
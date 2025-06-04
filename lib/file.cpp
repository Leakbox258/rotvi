#include <cursor.hpp>

std::unique_ptr<cursor> fileWriteIn(const string &filename) {
    std::ifstream infile(filename);

    if (infile) {
        // load file

        auto loadedCursor = std::make_unique<cursor>(filename, syntax::getColorFmt(filename));
        auto &_lines_buf_ = loadedCursor->_lines_buf_;
        auto &_file_name = loadedCursor->_file_name_;
        auto &_status_msg_ = loadedCursor->_status_msg_;

        string line;
        while (std::getline(infile, line)) {

            // Remove trailing \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            _lines_buf_.push_back(line);
        }

        if (_lines_buf_.empty()) { // Ensure there's at least one empty line if file is empty
            _lines_buf_.emplace_back("");
        }

        loadedCursor->_status_msg_ = "\"" + filename + "\" loaded.";

        infile.close();

        return loadedCursor;
    } else {
        // touch file

        auto touchedCursor = std::make_unique<cursor>(filename, syntax::getColorFmt(filename));

        touchedCursor->_status_msg_ = "New file: " + filename;

        return touchedCursor;
    }
}

std::unique_ptr<cursor> fileWriteIn() { return fileWriteIn("untitled" + std::to_string(anonymouseNr++)); }
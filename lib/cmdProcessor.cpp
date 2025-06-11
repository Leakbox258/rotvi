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
        // --- Page Up 逻辑 ---
        // 计算一个页面的高度（通常是文本区域的高度）
        int page_height = term_rows > 1 ? term_rows - 1 : 1;

        // 1. 将光标向上移动一个页面的距离
        _row_ -= page_height;
        // 边界检查：确保光标不会移动到文件顶部之上
        if (_row_ < 0) {
            _row_ = 0;
        }

        // 2. 更新视口的顶部，使其与新的光标位置对齐
        //    Page Up 操作后，光标通常会停留在新视口的顶部
        _win_top_row_ = _row_;
        // 再次进行边界检查（虽然在 _row_ < 0 时已处理，但为了稳健）
        if (_win_top_row_ < 0) {
            _win_top_row_ = 0;
        }

        // 3. 调整列位置，确保光标列在新行上是有效的
        //    如果新行比旧行短，光标列不能超出新行的长度
        if (_row_ < lineNr()) { // 确保行号有效
            _col_ = std::min(_col_, lineColNr(_row_));
        } else {
            // 如果文件为空或行号无效，列应为0
            _col_ = 0;
        }
    } else if (_cmd_buf_ == "down") {
        // --- Page Down 逻辑 ---
        int page_height = term_rows > 1 ? term_rows - 1 : 1;
        int last_line_index = lineNr() > 0 ? lineNr() - 1 : 0;

        // 1. 将光标向下移动一个页面的距离
        _row_ += page_height;
        // 边界检查：确保光标不会移动到文件末尾之后
        if (_row_ > last_line_index) {
            _row_ = last_line_index;
        }

        // 2. 更新视口的顶部，以确保新的光标位置可见
        //    Page Down 操作后，光标通常会停留在新视口的顶部
        _win_top_row_ = _row_;

        // 3. 对视口顶部进行边界检查，防止视口底部超出文件末尾
        //    一个视口至少需要 term_rows - 1 行（如果文件足够长）
        if (lineNr() > (unsigned)(term_rows - 1)) {
            int max_possible_top_row = lineNr() - (term_rows - 1);
            if (_win_top_row_ > max_possible_top_row) {
                _win_top_row_ = max_possible_top_row;
            }
        } else {
            // 如果文件总行数小于一页，视口顶部总是0
            _win_top_row_ = 0;
        }

        // 4. 调整列位置，确保光标列在新行上是有效的
        if (_row_ < lineNr()) { // 确保行号有效
            _col_ = std::min(_col_, lineColNr(_row_));
        } else {
            _col_ = 0;
        }
    } else {
        _status_msg_ = "Not an editor command: " + _cmd_buf_;
    }

    _cmd_buf_.clear();
}
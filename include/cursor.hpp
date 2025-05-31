#pragma once
#ifndef ROTVI_CURSOR_HPP
#define ROTVI_CURSOR_HPP

#include <api.hpp>
#include <color.hpp>
#include <map>
#include <tools.hpp>
#include <utility>
#include <vector>

enum Mode { NORMAL, INSERT, COMMAND_LINE };

struct config {
    Mode mode;
    bool autoChangLine;
};

class cursor;

inline int term_rows, term_cols;

inline config cursorDefaultCfg = {.mode = Mode::NORMAL, .autoChangLine = false};

inline int anonymouseNr = 0;

inline std::map<string, cursor> openedCursors;

class cursor {
private:
    string _file_name_{};
    string _status_msg_{};
    string _cmd_buf_{};

    std::vector<string> _lines_buf_; // file content buffer

    config _config_;

    int _row_; // position of current cursor
    int _col_;

    int _win_top_row_; // the bounder of this window
    int _win_left_col_;

public:
    explicit cursor(string _file = "untiled", config _cfg = cursorDefaultCfg) noexcept
        : _file_name_(std::move(_file)), _config_(_cfg) {
        _row_ = 0;
        _col_ = 0;
        _win_top_row_ = 0;
        _win_left_col_ = 0;
    }

    [[nodiscard]] const auto &lines() const { return _lines_buf_; }

    [[nodiscard]] const auto &status() const { return _status_msg_; }

    void renewStatus(string &&_new_status) { _status_msg_ = _new_status; }

    [[nodiscard]] Mode mode() const { return _config_.mode; }

    void chmode(Mode _new_mode) {
        _config_.mode = _new_mode;
        if (_config_.mode == Mode::NORMAL) {
            _status_msg_ = "-- NORMAL --";
        } else if (_config_.mode == Mode::INSERT) {
            _status_msg_ = "-- INSERT --";
        } else if (_config_.mode == Mode::COMMAND_LINE) {
            _status_msg_ = "-- COMMAND LINE --";
        }
    }

    [[nodiscard]] bool isAutoChangeline() const { return _config_.autoChangLine; }

    void renewFileName(const string &_new_filename) { _file_name_ = _new_filename; }

    void renewFileName(string &&_new_filename) { _file_name_ = _new_filename; }

    unsigned lineNr() { return _lines_buf_.size(); }

    bool lineEmpty() { return _lines_buf_.empty(); }

    int lineColNr(int _row) { return static_cast<int>(_lines_buf_.at(_row).size()); }

    auto &lineCur() { return _lines_buf_[_row_]; }
    [[nodiscard]] const auto &lineCur() const { return _lines_buf_[_row_]; }

    template <typename T> void lineAppend(T &&_content) { _lines_buf_.emplace_back(_content); }

    int mvRight() {
        if (_col_ == lineColNr(_row_)) {
            if (_config_.autoChangLine && _row_ < lineNr()) {
                ++_row_;
                return _col_ = 0;
            }

            return _col_;
        }

        return ++_col_;
    }

    int mvLeft() {
        if (_col_ == 0) {
            if (_config_.autoChangLine && _row_ > 0) {
                --_row_;
                return _col_ = lineColNr(_row_);
            }

            return _col_;
        }

        return --_col_;
    }

    int mvUp() {
        if (_row_ == 0) {
            return _row_;
        }

        --_row_;

        _col_ = std::min(_col_, lineColNr(_row_));

        return _row_;
    }

    int mvDown() {
        if (_row_ == lineNr() - 1) {
            return _row_;
        }

        ++_row_;

        _col_ = std::min(_col_, lineColNr(_row_));

        return _row_;
    }

    int mvHome() {
        return _col_ = 0; // NOLINT
    }

    int mvEnd() {
        if (_row_ < lineNr()) {
            return _col_ = lineColNr(_row_);
        }
        return _col_ = static_cast<int>(lineCur().length());
    }

    void setToEOF();

    void redrawScreen();

    void pressHandler(int ch);

    template <Mode M> void pressHandler(int ch);

    void cmdlineProcessor();

    void fileWriteBack(const std::string &filename);

    friend cursor fileWriteIn(const string &filename);

    ~cursor() = default;
};

cursor fileWriteIn(const string &filename);

cursor fileWriteIn();

#endif
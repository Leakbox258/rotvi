#pragma once
#ifndef ROTVI_CURSOR_HPP
#define ROTVI_CURSOR_HPP

#include <api.hpp>
#include <map>
#include <syntax/CXX/match.hpp>
///@todo add more...
#include <tools.hpp>
#include <utility>
#include <vector>

enum Mode { NORMAL, INSERT, COMMAND_LINE };

struct config {
    Mode mode;
    bool autoChangLine;
    unsigned indent;
};

class cursor;

inline int term_rows, term_cols;

inline config cursorDefaultCfg = {
    .mode = Mode::NORMAL,
    .autoChangLine = false,
    .indent = 8,
};

inline int anonymouseNr = 0;

class cursor {
public:
    friend class syntax::ColorFmtBase;
    template <syntax::fileType Type> friend class syntax::ColorFmt;

private:
    string _file_name_{};
    string _status_msg_{};
    string _cmd_buf_{};

    std::vector<string> _lines_buf_; // file content buffer

    config _config_;

    int _row_; // position of current cursor
    int _col_;
    bool moveIndent = false;

    int _win_top_row_; // the bounder of this window
    int _win_left_col_;

    bool modified = false;

    const syntax::ColorFmtBase &printer;

public:
    explicit cursor(string _file, const syntax::ColorFmtBase &_fmt, config _cfg = cursorDefaultCfg) noexcept
        : _file_name_(std::move(_file)), _config_(_cfg), printer(_fmt) {
        _row_ = 0;
        _col_ = 0;
        _win_top_row_ = 0;
        _win_left_col_ = 0;
    }

    [[nodiscard]] const auto &lines() const { return _lines_buf_; }

    [[nodiscard]] auto &lines() { return _lines_buf_; }

    [[nodiscard]] const auto &status() const { return _status_msg_; }

    void renewStatus(string &&_new_status) { _status_msg_ = _new_status; }

    [[nodiscard]] Mode mode() const { return _config_.mode; }

    void chmode(Mode _new_mode) {
        if (_new_mode == Mode::NORMAL && _config_.mode != Mode::COMMAND_LINE) {
            _status_msg_ = "-- NORMAL --";
        } else if (_new_mode == Mode::INSERT) {
            _status_msg_ = "-- INSERT --";
        } else if (_new_mode == Mode::COMMAND_LINE) {
            _status_msg_ = "-- COMMAND LINE --";
        }

        _config_.mode = _new_mode;
    }

    [[nodiscard]] bool isAutoChangeline() const { return _config_.autoChangLine; }

    void renewFileName(const string &_new_filename) { _file_name_ = _new_filename; }

    void renewFileName(string &&_new_filename) { _file_name_ = _new_filename; }

    unsigned lineNr() { return _lines_buf_.size(); }

    bool lineEmpty() { return _lines_buf_.empty(); }

    int top_row() const { return _win_top_row_; }

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

    int cmdmvLeft() {
        return _col_ = std::max(0, _col_ - 1); // NOLINT
    }

    int cmdmvRight() {
        return _col_ = std::min(static_cast<int>(_cmd_buf_.length()), _col_ + 1); // NOLINT
    }

    void setToEOF();

    void redrawScreen();

    void pressHandler(int ch);

    template <Mode M> void pressHandler(int ch);

    void cmdlineProcessor();

    void fileWriteBack(const std::string &filename);

    friend std::unique_ptr<cursor> fileWriteIn(const string &filename);

    friend int calScrCol(const cursor &);

    ~cursor() = default;
};

std::unique_ptr<cursor> fileWriteIn(const string &filename);

std::unique_ptr<cursor> fileWriteIn();

#endif
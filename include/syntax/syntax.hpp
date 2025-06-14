#pragma once
#ifndef ROTVI_SYNTAX_HPP
#define ROTVI_SYNTAX_HPP

#include <color.hpp>
#include <list>
#include <regex>
#include <vector>

inline string getExtension(const string &filename) {
    auto pos = filename.find_last_of('.');
    if (pos != string::npos && pos + 1 < filename.length()) {
        return filename.substr(pos + 1);
    }
    return "";
}
class cursor; // pre declaration

namespace syntax {

enum fileType : u_int32_t {
    UNKNOWN = 0,
    CXX
    ///@todo add more...
};

inline static std::map<string, fileType> fileTypeMap = {
    {"cpp", CXX}, {"cxx", CXX}, {"cc", CXX},  {"c", CXX},     {"h", CXX},     {"hpp", CXX}, {"hxx", CXX},
    {"hh", CXX},  {"cu", CXX},  {"cuh", CXX}, {"cuhpp", CXX}, {"cuhxx", CXX}, {"cuhh", CXX}};

struct token {
    const char *cstr;
    int y;
    int x;
    int len;
    enum attr : u_int32_t {
        None,
        Function,
        String,
        Comment,
    } tokenAttr;

    token(const char *const _cstr, int _y, int _x, int _len, attr _attr = None)
        : cstr(_cstr), y(_y), x(_x), len(_len), tokenAttr(_attr) {}
    ~token() = default;
};

template <typename T> inline std::vector<token> tokenlize(T &&raw_text) {

    char *token_flow_begin;
    std::vector<token> token_list;

    if constexpr (std::is_same_v<string, std::remove_cvref_t<T>>) {
        token_flow_begin = raw_text.c_str();
    } else if constexpr (std::is_same_v<char *, std::remove_cvref_t<T>>) {
        token_flow_begin = raw_text;
    } else {
        static_assert(false);
    }

    char *current_pos = token_flow_begin;
    char *last_pos = current_pos;
    std::vector<int> tab_pos{};
    int y_last = 0, x_last = 0;
    int y = 0, x = 0;

    auto last_token_attr = [&token_list]() -> std::optional<token::attr *> {
        if (token_list.empty()) {
            return std::nullopt;
        }
        return std::optional<token::attr *>(&token_list.back().tokenAttr);
    };

    auto getDisplayX = [&tab_pos](int _x) {
        int currentColumn = 0;
        const int tabWidth = 8;

        for (int i = 0; i < _x; ++i) {
            if (std::find(tab_pos.begin(), tab_pos.end(), i) != tab_pos.end()) {
                currentColumn = ((currentColumn / tabWidth) + 1) * tabWidth;
            } else {
                currentColumn += 1;
            }
        }

        return currentColumn;
    };

    while (*current_pos) {
        ///@brief Handle comments
        ///@note in comments, not neccessary to add tab_pos, because they'll be printed all in once
        ///@note but need to handle the tab before this comment in the same line
        if (*current_pos == '/' && *(current_pos + 1) == '/') {

            if (int len = static_cast<int>(current_pos - last_pos)) {
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), len);
            }

            x_last = x;

            int comment_len = 0;

            last_pos = current_pos;

            while (*current_pos && *current_pos != '\n') {
                current_pos += 1;
                ++x;
                ++comment_len;
            }

            ++comment_len;
            token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), comment_len, token::attr::Comment);

            x = 0, ++y, tab_pos.clear();
            y_last = y, x_last = x;
            current_pos += 1; // skip '\n'
            last_pos = current_pos;
            continue;
        } else if (*current_pos == '/' && *(current_pos + 1) == '*') {

            if (int len = static_cast<int>(current_pos - last_pos)) {
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), len);
            }

            x_last = x;

            int comment_len = 0;

            last_pos = current_pos;

            // Skip until we find the closing */
            while (*current_pos && !(*current_pos == '*' && *(current_pos + 1) == '/')) {
                *current_pos == '\t' ? (void)tab_pos.emplace_back(x) : nop;

                *current_pos == '\n' ? tab_pos.clear(), x = 0, ++y : ++x;

                current_pos += 1;

                ++comment_len;
            }

            if (*current_pos) {   // If we found the closing */
                current_pos += 2; // Skip past */
                x += 2;
                comment_len += 2;
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), comment_len, token::attr::Comment);

                last_pos = current_pos; // Reset for next comment
                y_last = y, x_last = x;
                continue;
            } else {
                // search */ but EOF
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), comment_len, token::attr::Comment);
                continue;
            }
        }

        ///@brief handle strings
        else if (*current_pos == '"') {

            if (int len = static_cast<int>(current_pos - last_pos)) {
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), len);
            }

            x_last = x;
            if (*(current_pos - 1) == '\\') {

                current_pos += 1;
                continue;
            }

            int string_len = 0;

            last_pos = current_pos;

            current_pos += 1;
            ++string_len, ++x;

            while (*current_pos && (*current_pos != '\"' || *(current_pos - 1) == '\\')) {
                *current_pos == '\n' ? x = 0, ++y : ++x;
                ++string_len;

                current_pos += 1;
            }

            if (*current_pos) {
                current_pos += 1;
                x += 1; // skip
                string_len += 1;
            }

            token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), string_len, token::attr::String);

            last_pos = current_pos;
            y_last = y, x_last = x;

            continue;
        }

        else if (inSet(*current_pos, '\t', '\r', '\n', ' ')) {
            token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), static_cast<int>(current_pos - last_pos));

            if (*current_pos == '\n') {
                x = 0, ++y;
                tab_pos.clear();
            } else if (*current_pos == '\t') {
                token_list.emplace_back(current_pos, y, getDisplayX(x), 1);
                tab_pos.emplace_back(x);
                ++x;
            } else {
                token_list.emplace_back(current_pos, y, getDisplayX(x), 1);
                ++x;
            }

            current_pos += 1;
            last_pos = current_pos;
            y_last = y, x_last = x;
            // } else if (inSet(*current_pos, '(', ')', '[', ']', '{', '}', '+', '-', '*', '!', '@', '#', '$', '%', '^', '&',
            //                  '=', '`', '~', '\\', ':', ';', '\'', '\"', ',', '<', '.', '>', '/', '?')) {
        } else if (inSet(*current_pos, '(', ')', '[', ']', '{', '}', '+', '*', '!', '@', '$', '%', '^', '&', '=', '`',
                         '~', '\\', ':', ';', ',', '<', '.', '>', '/', '?')) {
            if (int len = static_cast<int>(current_pos - last_pos)) {
                token_list.emplace_back(last_pos, y_last, getDisplayX(x_last), len);
            }

            if (auto ptr = last_token_attr(); *current_pos == '(' && ptr) {
                **ptr = token::Function;
            }

            token_list.emplace_back(current_pos, y, getDisplayX(x), 1);

            ++x;
            current_pos += 1;
            last_pos = current_pos;
            x_last = x;
        } else {
            current_pos += 1;
            ++x;
        }
    }

    return token_list;
}

template <fileType type>
inline std::pair<const api::ColorAttr &, const api::ColorAttr &> colorMatchImpl(const char *token_Cstr, int length,
                                                                                token::attr attr);

template <fileType type, typename T>
inline std::pair<const api::ColorAttr &, const api::ColorAttr &> colorMatch(T &&token, int length, token::attr attr) {
    const char *token_Cstr;

    if constexpr (std::is_same_v<string, std::remove_cvref_t<T>>) {
        token_Cstr = token.c_str();
    } else if constexpr (std::is_same_v<const char *, std::remove_cvref_t<T>> ||
                         std::is_same_v<char *, std::remove_cvref_t<T>>) {
        token_Cstr = token;
    } else {
        static_assert(false);
    }

    return colorMatchImpl<type>(token_Cstr, length, attr);
}

struct ColorFmtBase {
    virtual void operator()(const cursor &printCursor) const = 0;
};

template <fileType type> struct ColorFmt : public ColorFmtBase {
    void operator()(const cursor &printCursor) const override;
};

template <> struct ColorFmt<UNKNOWN> : public ColorFmtBase {
    void operator()(const cursor &printCursor) const override;
};

inline const ColorFmtBase &getColorFmt(const string &filename) {
    static ColorFmt<fileType::CXX> cxxFmt;
    static ColorFmt<fileType::UNKNOWN> unknownFmt;

    auto type = fileTypeMap[getExtension(filename)];

    switch (type) {
    case fileType::CXX:
        return cxxFmt;
        ///@todo add more...
    default:
        return unknownFmt;
    }

}; // namespace syntax
} // namespace syntax
#endif
#include <cstring>
#include <cursor.hpp>

using namespace syntax;

void ColorFmt<syntax::UNKNOWN>::operator()(const cursor &printCursor) const {
    for (int i = 0; i < term_rows - 1; ++i) { // -1 for status bar
        int doc_line_idx = printCursor._win_top_row_ + i;
        if (doc_line_idx < static_cast<int>(printCursor._lines_buf_.size())) {
            const string &line_to_draw = printCursor._lines_buf_[doc_line_idx];
            // Handle horizontal scrolling
            int len_to_draw = static_cast<int>(line_to_draw.size()) - printCursor._win_left_col_;
            if (len_to_draw > 0) {
                api::Mvprintw(i, 0, "%.*s", term_cols,
                              line_to_draw.substr(printCursor._win_left_col_, line_to_draw.length()).c_str());
            }
        }
    }
}

template <> void ColorFmt<syntax::CXX>::operator()(const cursor &printCursor) const {
    const auto &text_list = printCursor.lines();

    static_assert(is_sequential_counter<std::remove_cvref_t<decltype(text_list)>>::value);

    std::size_t reserve_size = 0;
    for (const auto &line : text_list) {
        reserve_size += line.c_str() ? strlen(line.c_str()) : 0;
    }

    reserve_size += text_list.size(); // for spaces
    reserve_size += 1;                // for '\0'

    auto raw_text = std::make_unique<char[]>(reserve_size);
    char *raw_text_ptr = raw_text.get();
    for (const auto &line : text_list) {
        if (line.c_str() == nullptr) {
            continue;
        }

        strncpy(raw_text_ptr, line.c_str(), line.length());
        strncat(raw_text_ptr, "\n", 1);
        raw_text_ptr += strlen(line.c_str()) + 1;
    }

    auto tokens = tokenlize(raw_text.get());

    for (auto &[token, y, x, len, attr] : tokens) { // use const auto & will be tricky

        auto color_pair = colorMatch<CXX>(token, len, attr);

        color_pair.first(); // color attr on

        if (y > printCursor._win_top_row_ + term_rows - 1) {
            break;
        }
        if (x > printCursor._win_left_col_ + term_cols) {
            continue;
        }

        api::Mvprintw(y, x, "%.*s", std::min(len, term_cols - x), token);

        color_pair.second(); // color attr off
    }
}
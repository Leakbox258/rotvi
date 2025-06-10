#pragma once
#ifndef ROTVI_SYNTAX_CXX_KEYWORDS_HPP
#define ROTVI_SYNTAX_CXX_KEYWORDS_HPP

#include <cstring>
#include <list>
#include <regex>
#include <tools.hpp>
#include <vector>

namespace CXX_syntax {

static const std::vector<const char *> keywords = { // NOLINT
    "alignas",       "alignof",          "and",       "and_eq",    "asm",          "auto",         "bool",
    "char",          "char8_t",          "char16_t",  "char32_t",  "class",        "compl",        "concept",
    "const",         "consteval",        "constexpr", "constinit", "const_cast",   "co_await",     "co_return",
    "co_yield",      "decltype",         "delete",    "double",    "dynamic_cast", "enum",         "explicit",
    "export",        "extern",           "false",     "float",     "friend",       "inline",       "int",
    "long",          "mutable",          "namespace", "new",       "noexcept",     "not",          "not_eq",
    "nullptr",       "operator",         "or",        "or_eq",     "private",      "protected",    "public",
    "register",      "reinterpret_cast", "requires",  "short",     "signed",       "sizeof",       "static",
    "static_assert", "static_cast",      "struct",    "template",  "this",         "thread_local", "true",
    "typedef",       "typeid",           "typename",  "union",     "unsigned",     "using",        "virtual",
    "void",          "volatile",         "wchar_t",   "xor",       "xor_eq"};

static const std::vector<const char *> keywords_flowcontrol = { // NOLINT
    "break", "case", "catch",  "continue", "default", "do",  "else", "for",
    "goto",  "if",   "return", "switch",   "throw",   "try", "while"};

static const std::regex numeric_rx(
    R"(^(0[xX][0-9a-fA-F]+[uUlL]{0,3}|0[0-7]*[uUlL]{0,3}|[1-9][0-9]*[uUlL]{0,3}|([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][+-]?[0-9]+)?[fFlL]?|[0-9]+[eE][+-]?[0-9]+[fFlL]?)$)");

// static const std::regex string_lit_rx(R"((?:u8|u|U|L)?\"(?:[^\"\\\n]|\\.)*\")");
static const std::regex string_lit_rx(R"((?:u8|u|U|L)?\"(?:[^\"\\]|\\[\s\S])*?\")");

static const std::regex char_lit_rx(R"(^(u8|u|U|L)?\'([^'\\]|\\.)\')");

static const std::regex identifier_rx(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");

static const std::regex comments_rx(R"(//.*|/\*[\s\S]*?\*/)");
}; // namespace CXX_syntax
#endif
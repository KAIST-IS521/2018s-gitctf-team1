#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);
std::string &strtolower(std::string &str);

// https://www.joinc.co.kr/w/Site/Code/C/urlencode
// https://developer.mozilla.org/ko/docs/Web/JavaScript/Reference/Global_Objects/encodeURIComponent
#define IS_ALNUM(ch) ( \
        ( ch >= 'a' && ch <= 'z' ) || \
        ( ch >= 'A' && ch <= 'Z' ) || \
        ( ch >= '0' && ch <= '9' ) || \
        ( ch == '!' ) || ( ch == '.' ) || ( ch == '~' ) || ( ch == '*' ) || \
        ( ch == '-' ) || ( ch == '_' ) || ( ch == '\'' ) || ( ch == '(' ) || \
        ( ch == ')' ) )
#define IS_HEX(ch) ( \
        ( ch >= '0' && ch <= '9' ) || \
        ( ch >= 'A' && ch <= 'F' ) || \
        ( ch >= 'a' && ch <= 'f' ))

std::string urlencode(std::string str);
std::string urldecode(std::string str);

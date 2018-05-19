#include "Util.h"

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string &strtolower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

std::string urlencode(std::string str) {
    int len;
    std::string ret;
    char tmp[4] = {0, };

    len = str.size();
    for (int i = 0; i < len; ++i) {
        if (IS_ALNUM(str[i]))
            ret += str[i];
        else {
            snprintf(tmp, 4, "%%%02X", (unsigned char)(str[i] & 0xFF));
            ret += tmp;
        }
    }

    return ret;
}

std::string urldecode(std::string str) {
    int len;
    std::string ret;
    char hex[3] = {0, };

    len = str.length();
    for (int i = 0; i < len; ++i) {
        if (str[i] != '%') {
            ret += str[i];
        } else {
            if (IS_HEX(str[i + 1]) && IS_HEX(str[i + 2]) && i < (len - 2)) {
                hex[0] = str[i + 1];
                hex[1] = str[i + 2];
                ret += ::strtol( hex, NULL, 16 );
                i += 2;
            } else {
                // invalid encoding
                ret.clear();
                break;
            }
        }
    }
    return ret;
}

std::string ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

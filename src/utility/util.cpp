#include "util.h"
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Util {
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }

    std::string to_lower(const std::string& str) {
        std::string result = str;
        for (char& c : result) c = std::tolower(c);
        return result;
    }

    std::string to_upper(const std::string& str) {
        std::string result = str;
        for (char& c : result) c = std::toupper(c);
        return result;
    }

    std::vector<std::string> split_string(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool is_digit(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }

    bool has_upper_case(const std::string& password) {
        return std::any_of(password.begin(), password.end(), ::isupper);
    }

    bool has_lower_case(const std::string& password) {
        return std::any_of(password.begin(), password.end(), ::islower);
    }

    bool has_digit(const std::string& password) {
        return std::any_of(password.begin(), password.end(), ::isdigit);
    }

    bool is_username_valid(const std::string& username) {
        return !username.empty() && username.size() <= 32 &&
               std::all_of(username.begin(), username.end(), [](char c) {
                   return std::isalnum(c) || c == '_' || c == '-';
               });
    }

    std::string char_to_hex(char c) {
        std::ostringstream hex;
        hex << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
        return hex.str();
    }

    std::string url_encode(const std::string& message) {
        std::ostringstream encoded;
        for (auto& v : message) {
            if (std::isalnum(v) || v == '-' || v == '_' || v == '~' || v == '.') {
                encoded << v;
            } else {
                encoded << '%' << char_to_hex(v);
            }
        }
        return encoded.str();
    }

    std::string url_decode(const std::string& message) {
        std::ostringstream decoded;
        for (size_t i = 0; i < message.size(); ++i) {
            char v = message[i];
            if (std::isalnum(v) || v == '-' || v == '_' || v == '.') {
                decoded << v;
            } else if (v == '+') {
                decoded << ' ';
            } else if (v == '%' && i + 2 < message.size()) {
                // %XY getting XY (substr) then converting hex number to ASCII then ASCII to char
                decoded << static_cast<char>(std::stoi(message.substr(i + 1, 2), nullptr, 16));
                i += 2;
            }
        }
        return decoded.str();
    }

    void make_error_response(int code, const std::string& message, HttpResponse& res) {
        res.set_status(code, message);
        res.set_header("Content-Type", "application/json");
        res.set_body("{\"error\": \"" + message + "\"}");
    }
}

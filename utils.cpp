#include "utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <chrono>
#include <ctime>
#include <unordered_set>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool isValidUserID(const std::string& userID) {
    if (userID.empty() || userID.length() > 30) return false;
    for (char c : userID) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool isValidPassword(const std::string& password) {
    if (password.empty() || password.length() > 30) return false;
    for (char c : password) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 30) return false;
    for (char c : username) {
        if (c < 32 || c > 126) return false; // ASCII printable characters
    }
    return true;
}

bool isValidPrivilege(const std::string& privilege) {
    if (privilege.length() != 1) return false;
    char p = privilege[0];
    return p == '1' || p == '3' || p == '7';
}

bool isValidISBN(const std::string& isbn) {
    if (isbn.empty() || isbn.length() > 20) return false;
    for (char c : isbn) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool isValidBookName(const std::string& name) {
    if (name.length() > 60) return false;
    for (char c : name) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool isValidAuthor(const std::string& author) {
    if (author.length() > 60) return false;
    for (char c : author) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool isValidKeyword(const std::string& keyword) {
    if (keyword.length() > 60) return false;
    for (char c : keyword) {
        if (c < 32 || c > 126 || c == '"') return false;
    }

    // Check for duplicate keywords
    std::vector<std::string> keywords = split(keyword, '|');
    std::unordered_set<std::string> uniqueKeywords;
    for (const auto& k : keywords) {
        if (k.empty()) return false;
        if (uniqueKeywords.count(k)) return false;
        uniqueKeywords.insert(k);
    }
    return true;
}

bool isValidQuantity(const std::string& quantity) {
    if (quantity.empty() || quantity.length() > 10) return false;
    for (char c : quantity) {
        if (!isdigit(c)) return false;
    }
    long long val = std::stoll(quantity);
    return val > 0 && val <= 2147483647;
}

bool isValidPrice(const std::string& price) {
    if (price.empty() || price.length() > 13) return false;
    bool hasDot = false;
    for (char c : price) {
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    double val = std::stod(price);
    return val >= 0;
}

bool isValidCount(const std::string& count) {
    if (count.empty() || count.length() > 10) return false;
    for (char c : count) {
        if (!isdigit(c)) return false;
    }
    long long val = std::stoll(count);
    return val >= 0 && val <= 2147483647;
}

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void ensureDirectoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Directory doesn't exist, create it
        #ifdef _WIN32
            _mkdir(path.c_str());
        #else
            mkdir(path.c_str(), 0755);
        #endif
    }
}

double stringToDouble(const std::string& str) {
    return std::stod(str);
}

int stringToInt(const std::string& str) {
    return std::stoi(str);
}

std::string doubleToString(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string result = ss.str();

    // Remove trailing zeros
    if (precision > 0) {
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        result.erase(result.find_last_not_of('.') + 1, std::string::npos);
    }

    return result;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
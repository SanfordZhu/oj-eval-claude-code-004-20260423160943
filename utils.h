#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>

// String utilities
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
bool isValidUserID(const std::string& userID);
bool isValidPassword(const std::string& password);
bool isValidUsername(const std::string& username);
bool isValidPrivilege(const std::string& privilege);
bool isValidISBN(const std::string& isbn);
bool isValidBookName(const std::string& name);
bool isValidAuthor(const std::string& author);
bool isValidKeyword(const std::string& keyword);
bool isValidQuantity(const std::string& quantity);
bool isValidPrice(const std::string& price);
bool isValidCount(const std::string& count);

// File utilities
bool fileExists(const std::string& filename);
void ensureDirectoryExists(const std::string& path);

// Number utilities
double stringToDouble(const std::string& str);
int stringToInt(const std::string& str);
std::string doubleToString(double value, int precision = 2);

// Time utilities
std::string getCurrentTimestamp();

#endif // UTILS_H
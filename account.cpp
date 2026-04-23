#include "account.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

Account::Account(const std::string& userID, const std::string& password,
                 const std::string& username, int privilege)
    : userID(userID), password(password), username(username),
      privilege(privilege), loggedIn(false) {}

AccountManager::AccountManager() : accountsFile("accounts.dat") {
    initialize();
}

AccountManager::~AccountManager() {
    saveAccounts();
}

void AccountManager::initialize() {
    loadAccounts();

    // Create root account if it doesn't exist
    if (!userExists("root")) {
        createRootAccount();
    }
}

void AccountManager::createRootAccount() {
    auto root = std::make_shared<Account>("root", "sjtu", "root", 7);
    accounts["root"] = root;
    saveAccounts();
}

bool AccountManager::login(const std::string& userID, const std::string& password) {
    auto it = accounts.find(userID);
    if (it == accounts.end()) {
        return false;
    }

    auto currentUser = getCurrentUser();

    // Save current user's selected book before switching
    if (currentUser) {
        // We need access to bookManager here, but it's not available in AccountManager
        // This is a design issue - we need to handle this at a higher level
    }

    // If current user has higher privilege, password can be omitted
    if (currentUser && currentUser->getPrivilege() > it->second->getPrivilege()) {
        it->second->setLoggedIn(true);
        loginStack.push(it->second);
        return true;
    }

    // Check password
    if (it->second->getPassword() != password) {
        return false;
    }

    it->second->setLoggedIn(true);
    loginStack.push(it->second);
    return true;
}

bool AccountManager::logout() {
    if (loginStack.empty()) {
        return false;
    }

    auto user = loginStack.top();
    user->setLoggedIn(false);
    loginStack.pop();
    return true;
}

bool AccountManager::registerUser(const std::string& userID, const std::string& password,
                                 const std::string& username) {
    if (userExists(userID)) {
        return false;
    }

    if (!isValidUserID(userID) || !isValidPassword(password) || !isValidUsername(username)) {
        return false;
    }

    auto newUser = std::make_shared<Account>(userID, password, username, 1);
    accounts[userID] = newUser;
    saveAccounts();
    return true;
}

bool AccountManager::changePassword(const std::string& userID, const std::string& currentPassword,
                                   const std::string& newPassword) {
    auto it = accounts.find(userID);
    if (it == accounts.end()) {
        return false;
    }

    auto currentUser = getCurrentUser();

    // If current user has privilege 7, current password can be omitted
    if (!currentUser || currentUser->getPrivilege() < 7) {
        if (it->second->getPassword() != currentPassword) {
            return false;
        }
    }

    if (!isValidPassword(newPassword)) {
        return false;
    }

    it->second->setPassword(newPassword);
    saveAccounts();
    return true;
}

bool AccountManager::addUser(const std::string& userID, const std::string& password,
                            int privilege, const std::string& username) {
    if (userExists(userID)) {
        return false;
    }

    auto currentUser = getCurrentUser();
    if (!currentUser || currentUser->getPrivilege() <= privilege) {
        return false;
    }

    if (!isValidUserID(userID) || !isValidPassword(password) ||
        !isValidUsername(username) || privilege < 1 || privilege > 7) {
        return false;
    }

    auto newUser = std::make_shared<Account>(userID, password, username, privilege);
    accounts[userID] = newUser;
    saveAccounts();
    return true;
}

bool AccountManager::deleteUser(const std::string& userID) {
    if (userID == "root") {
        return false;
    }

    auto it = accounts.find(userID);
    if (it == accounts.end()) {
        return false;
    }

    // Check if user is logged in (anywhere in the stack)
    std::stack<std::shared_ptr<Account>> tempStack;
    bool isLoggedIn = false;

    // Check current login stack
    while (!loginStack.empty()) {
        auto user = loginStack.top();
        loginStack.pop();
        if (user->getUserID() == userID) {
            isLoggedIn = true;
        }
        tempStack.push(user);
    }

    // Restore login stack
    while (!tempStack.empty()) {
        loginStack.push(tempStack.top());
        tempStack.pop();
    }

    if (isLoggedIn) {
        return false;
    }

    accounts.erase(it);
    saveAccounts();
    return true;
}

std::shared_ptr<Account> AccountManager::getCurrentUser() const {
    if (loginStack.empty()) {
        return nullptr;
    }
    return loginStack.top();
}

bool AccountManager::userExists(const std::string& userID) const {
    return accounts.find(userID) != accounts.end();
}

std::shared_ptr<Account> AccountManager::getUser(const std::string& userID) const {
    auto it = accounts.find(userID);
    if (it != accounts.end()) {
        return it->second;
    }
    return nullptr;
}

void AccountManager::loadAccounts() {
    std::ifstream file(accountsFile, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    accounts.clear();

    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    // Sanity check to prevent corrupted files from causing issues
    if (count > 100000) {
        file.close();
        return;
    }

    for (size_t i = 0; i < count && file.good(); ++i) {
        size_t idLen = 0, passLen = 0, nameLen = 0;
        int privilege = 0;
        bool loggedIn = false;

        file.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
        if (idLen > 100) { // Sanity check
            break;
        }
        std::string userID(idLen, '\0');
        file.read(&userID[0], idLen);

        file.read(reinterpret_cast<char*>(&passLen), sizeof(passLen));
        if (passLen > 100) { // Sanity check
            break;
        }
        std::string password(passLen, '\0');
        file.read(&password[0], passLen);

        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        if (nameLen > 100) { // Sanity check
            break;
        }
        std::string username(nameLen, '\0');
        file.read(&username[0], nameLen);

        file.read(reinterpret_cast<char*>(&privilege), sizeof(privilege));
        file.read(reinterpret_cast<char*>(&loggedIn), sizeof(loggedIn));

        // Validate data before creating account
        if (isValidUserID(userID) && isValidPassword(password) && isValidUsername(username) &&
            (privilege == 1 || privilege == 3 || privilege == 7)) {
            auto account = std::make_shared<Account>(userID, password, username, privilege);
            account->setLoggedIn(loggedIn);
            accounts[userID] = account;
        }
    }

    file.close();
}

void AccountManager::saveAccounts() const {
    std::ofstream file(accountsFile, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    size_t count = accounts.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& pair : accounts) {
        const auto& account = pair.second;

        size_t idLen = account->getUserID().length();
        size_t passLen = account->getPassword().length();
        size_t nameLen = account->getUsername().length();
        int privilege = account->getPrivilege();
        bool loggedIn = account->isLoggedIn();

        file.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
        file.write(account->getUserID().c_str(), idLen);

        file.write(reinterpret_cast<const char*>(&passLen), sizeof(passLen));
        file.write(account->getPassword().c_str(), passLen);

        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(account->getUsername().c_str(), nameLen);

        file.write(reinterpret_cast<const char*>(&privilege), sizeof(privilege));
        file.write(reinterpret_cast<const char*>(&loggedIn), sizeof(loggedIn));
    }

    file.close();
}
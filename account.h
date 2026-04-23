#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

class Account {
public:
    Account(const std::string& userID, const std::string& password,
            const std::string& username, int privilege);

    // Getters
    std::string getUserID() const { return userID; }
    std::string getPassword() const { return password; }
    std::string getUsername() const { return username; }
    int getPrivilege() const { return privilege; }

    // Setters
    void setPassword(const std::string& newPassword) { password = newPassword; }
    void setUsername(const std::string& newUsername) { username = newUsername; }
    void setPrivilege(int newPrivilege) { privilege = newPrivilege; }

    // Check if account is logged in
    bool isLoggedIn() const { return loggedIn; }
    void setLoggedIn(bool status) { loggedIn = status; }

private:
    std::string userID;
    std::string password;
    std::string username;
    int privilege;
    bool loggedIn;
};

class AccountManager {
public:
    AccountManager();
    ~AccountManager();

    // Initialize account system (create root account if not exists)
    void initialize();

    // Account operations
    bool login(const std::string& userID, const std::string& password);
    bool logout();
    bool registerUser(const std::string& userID, const std::string& password,
                     const std::string& username);
    bool changePassword(const std::string& userID, const std::string& currentPassword,
                       const std::string& newPassword);
    bool addUser(const std::string& userID, const std::string& password,
                int privilege, const std::string& username);
    bool deleteUser(const std::string& userID);

    // Get current user
    std::shared_ptr<Account> getCurrentUser() const;

    // Check if user exists
    bool userExists(const std::string& userID) const;

    // Get user by ID
    std::shared_ptr<Account> getUser(const std::string& userID) const;

    // Load/Save accounts
    void loadAccounts();
    void saveAccounts() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Account>> accounts;
    std::stack<std::shared_ptr<Account>> loginStack;
    std::string accountsFile;

    // Helper functions
    void createRootAccount();
    std::string encryptPassword(const std::string& password) const;
    bool verifyPassword(const std::string& password, const std::string& encrypted) const;
};

#endif // ACCOUNT_H
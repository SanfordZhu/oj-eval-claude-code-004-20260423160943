#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <memory>
#include <deque>

enum class LogType {
    FINANCE,
    OPERATION,
    EMPLOYEE
};

class FinanceLog {
public:
    FinanceLog(const std::string& timestamp, double income, double expenditure,
               const std::string& description);

    std::string getTimestamp() const { return timestamp; }
    double getIncome() const { return income; }
    double getExpenditure() const { return expenditure; }
    std::string getDescription() const { return description; }
    double getNetAmount() const { return income - expenditure; }

    std::string toString() const;

private:
    std::string timestamp;
    double income;
    double expenditure;
    std::string description;
};

class OperationLog {
public:
    OperationLog(const std::string& timestamp, const std::string& userID,
                 const std::string& operation, const std::string& details);

    std::string getTimestamp() const { return timestamp; }
    std::string getUserID() const { return userID; }
    std::string getOperation() const { return operation; }
    std::string getDetails() const { return details; }

    std::string toString() const;

private:
    std::string timestamp;
    std::string userID;
    std::string operation;
    std::string details;
};

class EmployeeLog {
public:
    EmployeeLog(const std::string& userID, const std::string& username,
                int operationsCount, double totalIncome, double totalExpenditure);

    std::string getUserID() const { return userID; }
    std::string getUsername() const { return username; }
    int getOperationsCount() const { return operationsCount; }
    double getTotalIncome() const { return totalIncome; }
    double getTotalExpenditure() const { return totalExpenditure; }

    std::string toString() const;

private:
    std::string userID;
    std::string username;
    int operationsCount;
    double totalIncome;
    double totalExpenditure;
};

class LogManager {
public:
    LogManager();
    ~LogManager();

    // Finance log operations
    void addFinanceLog(double income, double expenditure, const std::string& description);
    std::vector<std::shared_ptr<FinanceLog>> getFinanceLogs(int count = -1) const;
    double getTotalIncome() const;
    double getTotalExpenditure() const;

    // Operation log operations
    void addOperationLog(const std::string& userID, const std::string& operation,
                        const std::string& details);
    std::vector<std::shared_ptr<OperationLog>> getOperationLogs() const;

    // Employee report generation
    std::vector<std::shared_ptr<EmployeeLog>> generateEmployeeReport() const;

    // Load/Save logs
    void loadLogs();
    void saveLogs() const;

private:
    std::deque<std::shared_ptr<FinanceLog>> financeLogs;
    std::vector<std::shared_ptr<OperationLog>> operationLogs;
    std::string financeLogFile;
    std::string operationLogFile;

    // Helper functions
    void ensureLogDirectoryExists();
    std::string formatFinanceSummary(double income, double expenditure) const;
};

#endif // LOG_H
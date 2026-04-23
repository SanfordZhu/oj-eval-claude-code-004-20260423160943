#include "log.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

FinanceLog::FinanceLog(const std::string& timestamp, double income,
                       double expenditure, const std::string& description)
    : timestamp(timestamp), income(income), expenditure(expenditure),
      description(description) {}

std::string FinanceLog::toString() const {
    std::stringstream ss;
    ss << timestamp << " " << std::fixed << std::setprecision(2)
       << "+" << income << " -" << expenditure << " " << description;
    return ss.str();
}

OperationLog::OperationLog(const std::string& timestamp, const std::string& userID,
                           const std::string& operation, const std::string& details)
    : timestamp(timestamp), userID(userID), operation(operation), details(details) {}

std::string OperationLog::toString() const {
    return timestamp + " " + userID + " " + operation + " " + details;
}

EmployeeLog::EmployeeLog(const std::string& userID, const std::string& username,
                         int operationsCount, double totalIncome, double totalExpenditure)
    : userID(userID), username(username), operationsCount(operationsCount),
      totalIncome(totalIncome), totalExpenditure(totalExpenditure) {}

std::string EmployeeLog::toString() const {
    std::stringstream ss;
    ss << "Employee: " << username << " (" << userID << ")\n"
       << "Operations: " << operationsCount << "\n"
       << "Total Income: " << std::fixed << std::setprecision(2) << totalIncome << "\n"
       << "Total Expenditure: " << totalExpenditure << "\n"
       << "Net: " << (totalIncome - totalExpenditure);
    return ss.str();
}

LogManager::LogManager() : financeLogFile("finance.log"), operationLogFile("operation.log") {
    loadLogs();
}

LogManager::~LogManager() {
    saveLogs();
}

void LogManager::addFinanceLog(double income, double expenditure, const std::string& description) {
    auto log = std::make_shared<FinanceLog>(
        getCurrentTimestamp(), income, expenditure, description);
    financeLogs.push_back(log);

    // Keep only last 10000 logs to prevent memory issues
    if (financeLogs.size() > 10000) {
        financeLogs.pop_front();
    }
}

std::vector<std::shared_ptr<FinanceLog>> LogManager::getFinanceLogs(int count) const {
    std::vector<std::shared_ptr<FinanceLog>> result;

    if (count < 0 || count > static_cast<int>(financeLogs.size())) {
        count = financeLogs.size();
    }

    auto it = financeLogs.rbegin();
    for (int i = 0; i < count && it != financeLogs.rend(); ++i, ++it) {
        result.push_back(*it);
    }

    return result;
}

double LogManager::getTotalIncome() const {
    double total = 0.0;
    for (const auto& log : financeLogs) {
        total += log->getIncome();
    }
    return total;
}

double LogManager::getTotalExpenditure() const {
    double total = 0.0;
    for (const auto& log : financeLogs) {
        total += log->getExpenditure();
    }
    return total;
}

void LogManager::addOperationLog(const std::string& userID, const std::string& operation,
                                const std::string& details) {
    auto log = std::make_shared<OperationLog>(
        getCurrentTimestamp(), userID, operation, details);
    operationLogs.push_back(log);

    // Keep only last 10000 logs to prevent memory issues
    if (operationLogs.size() > 10000) {
        operationLogs.erase(operationLogs.begin());
    }
}

std::vector<std::shared_ptr<OperationLog>> LogManager::getOperationLogs() const {
    return operationLogs;
}

std::vector<std::shared_ptr<EmployeeLog>> LogManager::generateEmployeeReport() const {
    std::unordered_map<std::string, std::shared_ptr<EmployeeLog>> employeeStats;

    // Aggregate operation logs by user
    for (const auto& log : operationLogs) {
        const std::string& userID = log->getUserID();

        if (employeeStats.find(userID) == employeeStats.end()) {
            employeeStats[userID] = std::make_shared<EmployeeLog>(
                userID, userID, 0, 0.0, 0.0);
        }

        employeeStats[userID] = std::make_shared<EmployeeLog>(
            userID,
            employeeStats[userID]->getUsername(),
            employeeStats[userID]->getOperationsCount() + 1,
            employeeStats[userID]->getTotalIncome(),
            employeeStats[userID]->getTotalExpenditure()
        );
    }

    // Aggregate finance logs by user (simplified - assuming user is in description)
    for (const auto& log : financeLogs) {
        const std::string& description = log->getDescription();
        // Extract userID from description (simplified parsing)
        size_t pos = description.find("User:");
        if (pos != std::string::npos) {
            size_t endPos = description.find(" ", pos + 5);
            std::string userID = description.substr(pos + 5, endPos - pos - 5);

            if (employeeStats.find(userID) != employeeStats.end()) {
                employeeStats[userID] = std::make_shared<EmployeeLog>(
                    userID,
                    employeeStats[userID]->getUsername(),
                    employeeStats[userID]->getOperationsCount(),
                    employeeStats[userID]->getTotalIncome() + log->getIncome(),
                    employeeStats[userID]->getTotalExpenditure() + log->getExpenditure()
                );
            }
        }
    }

    std::vector<std::shared_ptr<EmployeeLog>> result;
    for (const auto& pair : employeeStats) {
        result.push_back(pair.second);
    }

    // Sort by userID
    std::sort(result.begin(), result.end(),
              [](const std::shared_ptr<EmployeeLog>& a, const std::shared_ptr<EmployeeLog>& b) {
                  return a->getUserID() < b->getUserID();
              });

    return result;
}

void LogManager::loadLogs() {
    // Load finance logs
    std::ifstream financeFile(financeLogFile);
    if (financeFile.is_open()) {
        std::string line;
        while (std::getline(financeFile, line)) {
            std::istringstream iss(line);
            std::string timestamp, incomeStr, expenditureStr, description;
            double income, expenditure;

            iss >> timestamp >> incomeStr >> expenditureStr;
            std::getline(iss, description);
            description = trim(description);

            // Parse income and expenditure
            income = std::stod(incomeStr.substr(1)); // Remove '+'
            expenditure = std::stod(expenditureStr.substr(1)); // Remove '-'

            auto log = std::make_shared<FinanceLog>(timestamp, income, expenditure, description);
            financeLogs.push_back(log);
        }
        financeFile.close();
    }

    // Load operation logs
    std::ifstream operationFile(operationLogFile);
    if (operationFile.is_open()) {
        std::string line;
        while (std::getline(operationFile, line)) {
            std::istringstream iss(line);
            std::string timestamp, userID, operation;
            std::string details;

            iss >> timestamp >> userID >> operation;
            std::getline(iss, details);
            details = trim(details);

            auto log = std::make_shared<OperationLog>(timestamp, userID, operation, details);
            operationLogs.push_back(log);
        }
        operationFile.close();
    }
}

void LogManager::saveLogs() const {
    // Save finance logs
    std::ofstream financeFile(financeLogFile);
    if (financeFile.is_open()) {
        for (const auto& log : financeLogs) {
            financeFile << log->toString() << "\n";
        }
        financeFile.close();
    }

    // Save operation logs
    std::ofstream operationFile(operationLogFile);
    if (operationFile.is_open()) {
        for (const auto& log : operationLogs) {
            operationFile << log->toString() << "\n";
        }
        operationFile.close();
    }
}

std::string LogManager::formatFinanceSummary(double income, double expenditure) const {
    std::stringstream ss;
    ss << "+ " << std::fixed << std::setprecision(2) << income
       << " - " << expenditure;
    return ss.str();
}
#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <memory>
#include "account.h"
#include "book.h"
#include "log.h"

enum class CommandType {
    // Basic commands
    QUIT,
    EXIT,

    // Account commands
    SU,
    LOGOUT,
    REGISTER,
    PASSWD,
    USERADD,
    DELETE,

    // Book commands
    SHOW,
    BUY,
    SELECT,
    MODIFY,
    IMPORT,

    // Log commands
    SHOW_FINANCE,
    LOG,
    REPORT_FINANCE,
    REPORT_EMPLOYEE,

    // Invalid command
    INVALID
};

class Command {
public:
    Command(CommandType type, const std::vector<std::string>& args);

    CommandType getType() const { return type; }
    const std::vector<std::string>& getArgs() const { return args; }
    std::string getArg(size_t index) const;
    size_t getArgCount() const { return args.size(); }

private:
    CommandType type;
    std::vector<std::string> args;
};

class CommandParser {
public:
    static std::shared_ptr<Command> parse(const std::string& line);

private:
    static CommandType parseCommandType(const std::string& cmd);
    static std::vector<std::string> tokenize(const std::string& line);
    static std::string extractQuotedString(const std::string& str, size_t& pos);
    static std::string extractParameter(const std::string& str, size_t& pos);
};

class CommandExecutor {
public:
    CommandExecutor(std::shared_ptr<AccountManager> accountManager,
                   std::shared_ptr<BookManager> bookManager,
                   std::shared_ptr<LogManager> logManager);

    // Execute command
    std::string execute(const Command& command);

    // Check if command should terminate program
    bool shouldExit() const { return exitRequested; }

private:
    std::shared_ptr<AccountManager> accountManager;
    std::shared_ptr<BookManager> bookManager;
    std::shared_ptr<LogManager> logManager;
    bool exitRequested;

    // Command handlers
    std::string handleQuit();
    std::string handleSu(const std::vector<std::string>& args);
    std::string handleLogout();
    std::string handleRegister(const std::vector<std::string>& args);
    std::string handlePasswd(const std::vector<std::string>& args);
    std::string handleUseradd(const std::vector<std::string>& args);
    std::string handleDelete(const std::vector<std::string>& args);
    std::string handleShow(const std::vector<std::string>& args);
    std::string handleBuy(const std::vector<std::string>& args);
    std::string handleSelect(const std::vector<std::string>& args);
    std::string handleModify(const std::vector<std::string>& args);
    std::string handleImport(const std::vector<std::string>& args);
    std::string handleShowFinance(const std::vector<std::string>& args);
    std::string handleLog();
    std::string handleReportFinance();
    std::string handleReportEmployee();

    // Helper functions
    bool checkPrivilege(int requiredPrivilege);
    std::string formatBookList(const std::vector<std::shared_ptr<Book>>& books);
    std::string formatFinanceSummary(double income, double expenditure);
};

#endif // COMMAND_H
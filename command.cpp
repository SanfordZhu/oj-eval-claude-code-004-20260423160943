#include "command.h"
#include "utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <set>

Command::Command(CommandType type, const std::vector<std::string>& args)
    : type(type), args(args) {}

std::string Command::getArg(size_t index) const {
    if (index < args.size()) {
        return args[index];
    }
    return "";
}

std::shared_ptr<Command> CommandParser::parse(const std::string& line) {
    std::string trimmedLine = trim(line);
    if (trimmedLine.empty()) {
        // Empty command is valid but does nothing
        return std::make_shared<Command>(CommandType::INVALID, std::vector<std::string>());
    }

    std::vector<std::string> tokens = tokenize(trimmedLine);
    if (tokens.empty()) {
        return std::make_shared<Command>(CommandType::INVALID, std::vector<std::string>());
    }

    // Handle composite commands
    if (tokens[0] == "show" && tokens.size() > 1 && tokens[1] == "finance") {
        // show finance [count]
        std::vector<std::string> args;
        if (tokens.size() > 2) {
            args.push_back(tokens[2]);
        }
        return std::make_shared<Command>(CommandType::SHOW_FINANCE, args);
    } else if (tokens[0] == "report" && tokens.size() > 1) {
        if (tokens[1] == "finance") {
            return std::make_shared<Command>(CommandType::REPORT_FINANCE, std::vector<std::string>());
        } else if (tokens[1] == "employee") {
            return std::make_shared<Command>(CommandType::REPORT_EMPLOYEE, std::vector<std::string>());
        }
    }

    CommandType type = parseCommandType(tokens[0]);
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    return std::make_shared<Command>(type, args);
}

CommandType CommandParser::parseCommandType(const std::string& cmd) {
    if (cmd == "quit") return CommandType::QUIT;
    if (cmd == "exit") return CommandType::EXIT;
    if (cmd == "su") return CommandType::SU;
    if (cmd == "logout") return CommandType::LOGOUT;
    if (cmd == "register") return CommandType::REGISTER;
    if (cmd == "passwd") return CommandType::PASSWD;
    if (cmd == "useradd") return CommandType::USERADD;
    if (cmd == "delete") return CommandType::DELETE;
    if (cmd == "show") return CommandType::SHOW;
    if (cmd == "buy") return CommandType::BUY;
    if (cmd == "select") return CommandType::SELECT;
    if (cmd == "modify") return CommandType::MODIFY;
    if (cmd == "import") return CommandType::IMPORT;
    if (cmd == "log") return CommandType::LOG;
    if (cmd == "report") return CommandType::REPORT_FINANCE;

    return CommandType::INVALID;
}

std::vector<std::string> CommandParser::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"' && !inQuotes) {
            inQuotes = true;
            current += c; // Include opening quote
        } else if (c == '"' && inQuotes) {
            inQuotes = false;
            current += c; // Include closing quote
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

CommandExecutor::CommandExecutor(std::shared_ptr<AccountManager> accountManager,
                                std::shared_ptr<BookManager> bookManager,
                                std::shared_ptr<LogManager> logManager)
    : accountManager(accountManager), bookManager(bookManager),
      logManager(logManager), exitRequested(false) {}

std::string CommandExecutor::execute(const Command& command) {
    switch (command.getType()) {
        case CommandType::QUIT:
        case CommandType::EXIT:
            return handleQuit();

        case CommandType::SU:
            return handleSu(command.getArgs());

        case CommandType::LOGOUT:
            return handleLogout();

        case CommandType::REGISTER:
            return handleRegister(command.getArgs());

        case CommandType::PASSWD:
            return handlePasswd(command.getArgs());

        case CommandType::USERADD:
            return handleUseradd(command.getArgs());

        case CommandType::DELETE:
            return handleDelete(command.getArgs());

        case CommandType::SHOW:
            return handleShow(command.getArgs());

        case CommandType::BUY:
            return handleBuy(command.getArgs());

        case CommandType::SELECT:
            return handleSelect(command.getArgs());

        case CommandType::MODIFY:
            return handleModify(command.getArgs());

        case CommandType::IMPORT:
            return handleImport(command.getArgs());

        case CommandType::SHOW_FINANCE:
            return handleShowFinance(command.getArgs());

        case CommandType::LOG:
            return handleLog();

        case CommandType::REPORT_FINANCE:
            return handleReportFinance();

        case CommandType::REPORT_EMPLOYEE:
            return handleReportEmployee();

        case CommandType::INVALID:
        default:
            return "Invalid";
    }
}

std::string CommandExecutor::handleQuit() {
    exitRequested = true;
    return "";
}

std::string CommandExecutor::handleSu(const std::vector<std::string>& args) {
    if (args.empty() || args.size() > 2) {
        return "Invalid";
    }

    std::string userID = args[0];
    std::string password = args.size() > 1 ? args[1] : "";

    if (!accountManager->login(userID, password)) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handleLogout() {
    if (!checkPrivilege(1)) {
        return "Invalid";
    }

    if (!accountManager->logout()) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handleRegister(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return "Invalid";
    }

    std::string userID = args[0];
    std::string password = args[1];
    std::string username = args[2];

    if (!accountManager->registerUser(userID, password, username)) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handlePasswd(const std::vector<std::string>& args) {
    if (args.size() < 2 || args.size() > 3) {
        return "Invalid";
    }

    if (!checkPrivilege(1)) {
        return "Invalid";
    }

    std::string userID = args[0];
    std::string currentPassword;
    std::string newPassword;

    if (args.size() == 2) {
        newPassword = args[1];
    } else {
        currentPassword = args[1];
        newPassword = args[2];
    }

    if (!accountManager->changePassword(userID, currentPassword, newPassword)) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handleUseradd(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        return "Invalid";
    }

    if (!checkPrivilege(3)) {
        return "Invalid";
    }

    std::string userID = args[0];
    std::string password = args[1];
    int privilege = stringToInt(args[2]);
    std::string username = args[3];

    if (!accountManager->addUser(userID, password, privilege, username)) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handleDelete(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "Invalid";
    }

    if (!checkPrivilege(7)) {
        return "Invalid";
    }

    std::string userID = args[0];

    if (!accountManager->deleteUser(userID)) {
        return "Invalid";
    }

    return "";
}

std::string CommandExecutor::handleShow(const std::vector<std::string>& args) {
    if (!checkPrivilege(1)) {
        return "Invalid";
    }

    std::vector<std::shared_ptr<Book>> books;

    if (args.empty()) {
        // Show all books
        books = bookManager->getAllBooks();
    } else if (args.size() == 1) {
        // Parse parameter
        std::string param = args[0];
        if (param.find("-ISBN=") == 0) {
            std::string isbn = param.substr(6);
            books = bookManager->searchByISBN(isbn);
        } else if (param.find("-name=\"") == 0 && param.back() == '"') {
            std::string name = param.substr(7, param.length() - 8);
            books = bookManager->searchByName(name);
        } else if (param.find("-author=\"") == 0 && param.back() == '"') {
            std::string author = param.substr(9, param.length() - 10);
            books = bookManager->searchByAuthor(author);
        } else if (param.find("-keyword=\"") == 0 && param.back() == '"') {
            std::string keyword = param.substr(10, param.length() - 11);
            books = bookManager->searchByKeyword(keyword);
        } else {
            return "Invalid";
        }
    } else {
        return "Invalid";
    }

    if (books.empty()) {
        return ""; // Return empty string which will output as empty line
    }

    return formatBookList(books);
}

std::string CommandExecutor::handleBuy(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "Invalid";
    }

    if (!checkPrivilege(1)) {
        return "Invalid";
    }

    std::string isbn = args[0];
    int quantity = stringToInt(args[1]);

    double totalCost;
    if (!bookManager->buyBook(isbn, quantity, totalCost)) {
        return "Invalid";
    }

    // Log the transaction
    auto currentUser = accountManager->getCurrentUser();
    std::string userID = currentUser ? currentUser->getUserID() : "unknown";
    std::string description = "User:" + userID + " bought " + std::to_string(quantity) +
                             " copies of ISBN:" + isbn;
    logManager->addFinanceLog(totalCost, 0.0, description);
    logManager->addOperationLog(userID, "buy", "ISBN:" + isbn + " Quantity:" + std::to_string(quantity));

    return doubleToString(totalCost);
}

std::string CommandExecutor::handleSelect(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "Invalid";
    }

    if (!checkPrivilege(3)) {
        return "Invalid";
    }

    std::string isbn = args[0];
    bookManager->setSelectedBook(isbn);

    return "";
}

std::string CommandExecutor::handleModify(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Invalid";
    }

    if (!checkPrivilege(3)) {
        return "Invalid";
    }

    auto selectedBook = bookManager->getSelectedBook();
    if (!selectedBook) {
        return "Invalid";
    }

    Book modifiedBook = *selectedBook;
    std::set<std::string> modifiedFields;

    for (const auto& arg : args) {
        if (arg.find("-ISBN=") == 0) {
            std::string newISBN = arg.substr(6);
            if (newISBN == selectedBook->getISBN()) {
                return "Invalid"; // Cannot change to same ISBN
            }
            if (bookManager->bookExists(newISBN)) {
                return "Invalid"; // ISBN already exists
            }
            modifiedBook.setISBN(newISBN);
            modifiedFields.insert("ISBN");
        } else if (arg.find("-name=\"") == 0 && arg.back() == '"') {
            std::string name = arg.substr(7, arg.length() - 8);
            if (name.empty()) return "Invalid";
            modifiedBook.setName(name);
            modifiedFields.insert("name");
        } else if (arg.find("-author=\"") == 0 && arg.back() == '"') {
            std::string author = arg.substr(9, arg.length() - 10);
            if (author.empty()) return "Invalid";
            modifiedBook.setAuthor(author);
            modifiedFields.insert("author");
        } else if (arg.find("-keyword=\"") == 0 && arg.back() == '"') {
            std::string keyword = arg.substr(10, arg.length() - 11);
            if (keyword.empty() || !isValidKeyword(keyword)) return "Invalid";
            modifiedBook.setKeyword(keyword);
            modifiedFields.insert("keyword");
        } else if (arg.find("-price=") == 0) {
            std::string priceStr = arg.substr(7);
            if (!isValidPrice(priceStr)) return "Invalid";
            double price = stringToDouble(priceStr);
            modifiedBook.setPrice(price);
            modifiedFields.insert("price");
        } else {
            return "Invalid";
        }
    }

    if (modifiedFields.empty()) {
        return "Invalid";
    }

    // Update the book
    if (modifiedFields.count("ISBN")) {
        // Delete old book and add new one
        bookManager->deleteBook(selectedBook->getISBN());
        bookManager->addBook(modifiedBook);
        bookManager->setSelectedBook(modifiedBook.getISBN());
    } else {
        bookManager->updateBook(modifiedBook);
    }

    // Log the operation
    auto currentUser = accountManager->getCurrentUser();
    std::string userID = currentUser ? currentUser->getUserID() : "unknown";
    std::string details = "ISBN:" + selectedBook->getISBN();
    for (const auto& field : modifiedFields) {
        details += " " + field + " modified";
    }
    logManager->addOperationLog(userID, "modify", details);

    return "";
}

std::string CommandExecutor::handleImport(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "Invalid";
    }

    if (!checkPrivilege(3)) {
        return "Invalid";
    }

    auto selectedBook = bookManager->getSelectedBook();
    if (!selectedBook) {
        return "Invalid";
    }

    int quantity = stringToInt(args[0]);
    double totalCost = stringToDouble(args[1]);

    if (!bookManager->importBook(selectedBook->getISBN(), quantity, totalCost)) {
        return "Invalid";
    }

    // Log the transaction
    auto currentUser = accountManager->getCurrentUser();
    std::string userID = currentUser ? currentUser->getUserID() : "unknown";
    std::string description = "User:" + userID + " imported " + std::to_string(quantity) +
                             " copies of ISBN:" + selectedBook->getISBN();
    logManager->addFinanceLog(0.0, totalCost, description);
    logManager->addOperationLog(userID, "import", "ISBN:" + selectedBook->getISBN() +
                               " Quantity:" + std::to_string(quantity) + " Cost:" + doubleToString(totalCost));

    return "";
}

std::string CommandExecutor::handleShowFinance(const std::vector<std::string>& args) {
    if (!checkPrivilege(7)) {
        return "Invalid";
    }

    int count = -1;
    if (!args.empty()) {
        count = stringToInt(args[0]);
        if (count < 0) {
            return "Invalid";
        }
    }

    auto logs = logManager->getFinanceLogs(count);
    double totalIncome = 0.0;
    double totalExpenditure = 0.0;

    for (const auto& log : logs) {
        totalIncome += log->getIncome();
        totalExpenditure += log->getExpenditure();
    }

    return formatFinanceSummary(totalIncome, totalExpenditure);
}

std::string CommandExecutor::handleLog() {
    if (!checkPrivilege(7)) {
        return "Invalid";
    }

    auto logs = logManager->getOperationLogs();
    std::string result;
    for (const auto& log : logs) {
        result += log->toString() + "\n";
    }

    // Remove trailing newline if exists
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

std::string CommandExecutor::handleReportFinance() {
    if (!checkPrivilege(7)) {
        return "Invalid";
    }

    double totalIncome = logManager->getTotalIncome();
    double totalExpenditure = logManager->getTotalExpenditure();

    std::stringstream ss;
    ss << "Financial Report\n";
    ss << "================\n";
    ss << "Total Income: " << std::fixed << std::setprecision(2) << totalIncome << "\n";
    ss << "Total Expenditure: " << totalExpenditure << "\n";
    ss << "Net Profit: " << (totalIncome - totalExpenditure) << "\n";
    ss << "================";

    return ss.str();
}

std::string CommandExecutor::handleReportEmployee() {
    if (!checkPrivilege(7)) {
        return "Invalid";
    }

    auto reports = logManager->generateEmployeeReport();
    std::stringstream ss;

    ss << "Employee Work Report\n";
    ss << "====================\n";

    for (const auto& report : reports) {
        ss << report->toString() << "\n\n";
    }

    std::string result = ss.str();
    if (!result.empty() && result.length() > 2) {
        result.erase(result.length() - 2); // Remove last two newlines
    }

    return result;
}

bool CommandExecutor::checkPrivilege(int requiredPrivilege) {
    auto currentUser = accountManager->getCurrentUser();
    if (!currentUser) {
        return requiredPrivilege == 0;
    }
    return currentUser->getPrivilege() >= requiredPrivilege;
}

std::string CommandExecutor::formatBookList(const std::vector<std::shared_ptr<Book>>& books) {
    if (books.empty()) {
        return "";
    }

    std::stringstream ss;
    for (const auto& book : books) {
        ss << book->toString() << "\n";
    }

    std::string result = ss.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

std::string CommandExecutor::formatFinanceSummary(double income, double expenditure) {
    std::stringstream ss;
    ss << "+ " << std::fixed << std::setprecision(2) << income
       << " - " << expenditure;
    return ss.str();
}
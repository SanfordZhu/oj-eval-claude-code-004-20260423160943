#include <iostream>
#include <string>
#include <memory>
#include "account.h"
#include "book.h"
#include "log.h"
#include "command.h"
#include "utils.h"

int main() {
    // Create managers
    auto accountManager = std::make_shared<AccountManager>();
    auto bookManager = std::make_shared<BookManager>();
    auto logManager = std::make_shared<LogManager>();

    // Create command executor
    CommandExecutor executor(accountManager, bookManager, logManager);

    // Main command loop
    std::string line;
    while (std::getline(std::cin, line)) {
        // Parse command
        auto command = CommandParser::parse(line);

        // Debug output
        // std::cerr << "DEBUG: Parsed command type: " << static_cast<int>(command->getType()) << std::endl;

        // Execute command
        std::string result = executor.execute(*command);

        // Output result
        if (!result.empty()) {
            std::cout << result;
            // Add newline if result doesn't end with one
            if (result.back() != '\n') {
                std::cout << std::endl;
            }
        } else if (command->getType() == CommandType::SHOW) {
            // For show command, output empty line when no results
            std::cout << std::endl;
        }

        // Check if should exit
        if (executor.shouldExit()) {
            break;
        }
    }

    return 0;
}
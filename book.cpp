#include "book.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

Book::Book(const std::string& isbn, const std::string& name,
           const std::string& author, const std::string& keyword,
           double price, int quantity)
    : isbn(isbn), name(name), author(author), keyword(keyword),
      price(price), quantity(quantity) {}

std::vector<std::string> Book::getKeywords() const {
    std::vector<std::string> keywords;
    std::stringstream ss(keyword);
    std::string token;
    while (std::getline(ss, token, '|')) {
        if (!token.empty()) {
            keywords.push_back(token);
        }
    }
    return keywords;
}

bool Book::hasKeyword(const std::string& searchKeyword) const {
    auto keywords = getKeywords();
    return std::find(keywords.begin(), keywords.end(), searchKeyword) != keywords.end();
}

std::string Book::toString() const {
    std::stringstream ss;
    ss << isbn << "\t" << name << "\t" << author << "\t" << keyword << "\t";
    ss << std::fixed << std::setprecision(2) << price << "\t" << quantity;
    return ss.str();
}

BookManager::BookManager() : booksFile("books.dat") {
    loadBooks();
}

BookManager::~BookManager() {
    saveBooks();
}

bool BookManager::addBook(const Book& book) {
    if (bookExists(book.getISBN())) {
        return false;
    }

    auto newBook = std::make_shared<Book>(book);
    books[book.getISBN()] = newBook;
    saveBooks();
    return true;
}

bool BookManager::updateBook(const Book& book) {
    auto it = books.find(book.getISBN());
    if (it == books.end()) {
        return false;
    }

    *it->second = book;
    saveBooks();
    return true;
}

bool BookManager::deleteBook(const std::string& isbn) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        return false;
    }

    books.erase(it);
    if (selectedBookISBN == isbn) {
        clearSelectedBook();
    }
    saveBooks();
    return true;
}

std::shared_ptr<Book> BookManager::getBook(const std::string& isbn) const {
    auto it = books.find(isbn);
    if (it != books.end()) {
        return it->second;
    }
    return nullptr;
}

bool BookManager::bookExists(const std::string& isbn) const {
    return books.find(isbn) != books.end();
}

std::vector<std::shared_ptr<Book>> BookManager::searchByISBN(const std::string& isbn) const {
    std::vector<std::shared_ptr<Book>> results;
    auto it = books.find(isbn);
    if (it != books.end()) {
        results.push_back(it->second);
    }
    return results;
}

std::vector<std::shared_ptr<Book>> BookManager::searchByName(const std::string& name) const {
    std::vector<std::shared_ptr<Book>> results;
    for (const auto& pair : books) {
        if (pair.second->getName().find(name) != std::string::npos) {
            results.push_back(pair.second);
        }
    }

    // Sort by ISBN
    std::sort(results.begin(), results.end(),
              [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                  return a->getISBN() < b->getISBN();
              });

    return results;
}

std::vector<std::shared_ptr<Book>> BookManager::searchByAuthor(const std::string& author) const {
    std::vector<std::shared_ptr<Book>> results;
    for (const auto& pair : books) {
        if (pair.second->getAuthor().find(author) != std::string::npos) {
            results.push_back(pair.second);
        }
    }

    // Sort by ISBN
    std::sort(results.begin(), results.end(),
              [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                  return a->getISBN() < b->getISBN();
              });

    return results;
}

std::vector<std::shared_ptr<Book>> BookManager::searchByKeyword(const std::string& keyword) const {
    std::vector<std::shared_ptr<Book>> results;
    for (const auto& pair : books) {
        if (pair.second->hasKeyword(keyword)) {
            results.push_back(pair.second);
        }
    }

    // Sort by ISBN
    std::sort(results.begin(), results.end(),
              [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                  return a->getISBN() < b->getISBN();
              });

    return results;
}

std::vector<std::shared_ptr<Book>> BookManager::getAllBooks() const {
    std::vector<std::shared_ptr<Book>> results;
    for (const auto& pair : books) {
        results.push_back(pair.second);
    }

    // Sort by ISBN
    std::sort(results.begin(), results.end(),
              [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                  return a->getISBN() < b->getISBN();
              });

    return results;
}

bool BookManager::buyBook(const std::string& isbn, int quantity, double& totalCost) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        return false;
    }

    if (quantity <= 0 || it->second->getQuantity() < quantity) {
        return false;
    }

    totalCost = quantity * it->second->getPrice();
    it->second->setQuantity(it->second->getQuantity() - quantity);
    saveBooks();
    return true;
}

bool BookManager::importBook(const std::string& isbn, int quantity, double totalCost) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        return false;
    }

    if (quantity <= 0 || totalCost < 0) {
        return false;
    }

    it->second->setQuantity(it->second->getQuantity() + quantity);
    saveBooks();
    return true;
}

void BookManager::setSelectedBook(const std::string& isbn) {
    if (!bookExists(isbn)) {
        // Create new book with only ISBN
        auto newBook = std::make_shared<Book>(isbn);
        books[isbn] = newBook;
        saveBooks();
    }
    selectedBookISBN = isbn;
}

std::shared_ptr<Book> BookManager::getSelectedBook() const {
    if (selectedBookISBN.empty()) {
        return nullptr;
    }
    return getBook(selectedBookISBN);
}

void BookManager::clearSelectedBook() {
    selectedBookISBN.clear();
}

void BookManager::saveSelectedBookForUser(const std::string& userID) {
    userSelectedBooks[userID] = selectedBookISBN;
}

void BookManager::restoreSelectedBookForUser(const std::string& userID) {
    auto it = userSelectedBooks.find(userID);
    if (it != userSelectedBooks.end()) {
        selectedBookISBN = it->second;
    } else {
        selectedBookISBN.clear();
    }
}

void BookManager::loadBooks() {
    std::ifstream file(booksFile, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    books.clear();

    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    // Sanity check to prevent corrupted files from causing issues
    if (count > 100000) {
        file.close();
        return;
    }

    for (size_t i = 0; i < count && file.good(); ++i) {
        size_t isbnLen = 0, nameLen = 0, authorLen = 0, keywordLen = 0;
        double price = 0.0;
        int quantity = 0;

        file.read(reinterpret_cast<char*>(&isbnLen), sizeof(isbnLen));
        if (isbnLen > 100) { // Sanity check
            break;
        }
        std::string isbn(isbnLen, '\0');
        file.read(&isbn[0], isbnLen);

        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        if (nameLen > 100) { // Sanity check
            break;
        }
        std::string name(nameLen, '\0');
        file.read(&name[0], nameLen);

        file.read(reinterpret_cast<char*>(&authorLen), sizeof(authorLen));
        if (authorLen > 100) { // Sanity check
            break;
        }
        std::string author(authorLen, '\0');
        file.read(&author[0], authorLen);

        file.read(reinterpret_cast<char*>(&keywordLen), sizeof(keywordLen));
        if (keywordLen > 100) { // Sanity check
            break;
        }
        std::string keyword(keywordLen, '\0');
        file.read(&keyword[0], keywordLen);

        file.read(reinterpret_cast<char*>(&price), sizeof(price));
        file.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));

        // Validate data before creating book
        if (isValidISBN(isbn) && isValidBookName(name) && isValidAuthor(author) &&
            isValidKeyword(keyword) && price >= 0 && quantity >= 0) {
            auto book = std::make_shared<Book>(isbn, name, author, keyword, price, quantity);
            books[isbn] = book;
        }
    }

    file.close();
}

void BookManager::saveBooks() const {
    std::ofstream file(booksFile, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    size_t count = books.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& pair : books) {
        const auto& book = pair.second;

        size_t isbnLen = book->getISBN().length();
        size_t nameLen = book->getName().length();
        size_t authorLen = book->getAuthor().length();
        size_t keywordLen = book->getKeyword().length();
        double price = book->getPrice();
        int quantity = book->getQuantity();

        file.write(reinterpret_cast<const char*>(&isbnLen), sizeof(isbnLen));
        file.write(book->getISBN().c_str(), isbnLen);

        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(book->getName().c_str(), nameLen);

        file.write(reinterpret_cast<const char*>(&authorLen), sizeof(authorLen));
        file.write(book->getAuthor().c_str(), authorLen);

        file.write(reinterpret_cast<const char*>(&keywordLen), sizeof(keywordLen));
        file.write(book->getKeyword().c_str(), keywordLen);

        file.write(reinterpret_cast<const char*>(&price), sizeof(price));
        file.write(reinterpret_cast<const char*>(&quantity), sizeof(quantity));
    }

    file.close();
}
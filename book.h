#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

class Book {
public:
    Book(const std::string& isbn = "", const std::string& name = "",
         const std::string& author = "", const std::string& keyword = "",
         double price = 0.0, int quantity = 0);

    // Getters
    std::string getISBN() const { return isbn; }
    std::string getName() const { return name; }
    std::string getAuthor() const { return author; }
    std::string getKeyword() const { return keyword; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    // Setters
    void setISBN(const std::string& newISBN) { isbn = newISBN; }
    void setName(const std::string& newName) { name = newName; }
    void setAuthor(const std::string& newAuthor) { author = newAuthor; }
    void setKeyword(const std::string& newKeyword) { keyword = newKeyword; }
    void setPrice(double newPrice) { price = newPrice; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }

    // Helper functions
    std::vector<std::string> getKeywords() const;
    bool hasKeyword(const std::string& keyword) const;
    std::string toString() const;

private:
    std::string isbn;
    std::string name;
    std::string author;
    std::string keyword;
    double price;
    int quantity;
};

class BookManager {
public:
    BookManager();
    ~BookManager();

    // Book operations
    bool addBook(const Book& book);
    bool updateBook(const Book& book);
    bool deleteBook(const std::string& isbn);
    std::shared_ptr<Book> getBook(const std::string& isbn) const;
    bool bookExists(const std::string& isbn) const;

    // Search operations
    std::vector<std::shared_ptr<Book>> searchByISBN(const std::string& isbn) const;
    std::vector<std::shared_ptr<Book>> searchByName(const std::string& name) const;
    std::vector<std::shared_ptr<Book>> searchByAuthor(const std::string& author) const;
    std::vector<std::shared_ptr<Book>> searchByKeyword(const std::string& keyword) const;
    std::vector<std::shared_ptr<Book>> getAllBooks() const;

    // Transaction operations
    bool buyBook(const std::string& isbn, int quantity, double& totalCost);
    bool importBook(const std::string& isbn, int quantity, double totalCost);

    // Selected book management
    void setSelectedBook(const std::string& isbn);
    std::shared_ptr<Book> getSelectedBook() const;
    void clearSelectedBook();

    // Login stack management - save/restore selected book per user
    void saveSelectedBookForUser(const std::string& userID);
    void restoreSelectedBookForUser(const std::string& userID);

    // Load/Save books
    void loadBooks();
    void saveBooks() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Book>> books;
    std::string selectedBookISBN;
    std::string booksFile;

    // Map to store selected book per user (for login stack)
    std::unordered_map<std::string, std::string> userSelectedBooks;

    // Helper functions
    std::vector<std::shared_ptr<Book>> filterBooks(
        std::function<bool(const Book&)> predicate) const;
};

#endif // BOOK_H
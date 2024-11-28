#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <cstdio>

using namespace std;

class Creds {
public:
    int UID;
    string username, password;
    bool isAdmin;

    Creds(int UID, string username, string password, bool isAdmin)
        : UID(UID), username(username), password(password), isAdmin(isAdmin) {}
};

class LogIn {
public:
    vector<Creds> credsInfo;
    Creds* currentUser = nullptr;

    void getCreds() {
        fstream loginFile("creds.csv");
        if (!loginFile.is_open()) {
            cerr << "Error: Unable to open creds.csv" << endl;
            return;
        }

        string line;
        while (getline(loginFile, line)) {
            int UID;
            string username, password, temp;
            bool isAdmin;

            stringstream inputString(line);
            getline(inputString, temp, ',');
            UID = atoi(temp.c_str());
            getline(inputString, username, ',');
            getline(inputString, password, ',');
            getline(inputString, temp, ',');
            istringstream(temp) >> isAdmin;

            credsInfo.emplace_back(UID, username, password, isAdmin);
        }
        loginFile.close();
    }

    void performLogin() {
        string enteredUsername, enteredPassword;
        cout << "Podaj nazwę użytkownika: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, enteredUsername);
        cout << enteredUsername << endl;

        cout << "Podaj hasło: ";
        getline(cin, enteredPassword);
        cout << enteredPassword << endl;

        for (auto& creds : credsInfo) {
            if (creds.username == enteredUsername && creds.password == enteredPassword) {
                currentUser = &creds;
                cout << "Zalogowano jako: " << creds.username << endl;
                return;
            }
        }

        cout << "Nieprawidłowa nazwa użytkownika lub hasło!" << endl;
        currentUser = nullptr;
    }

    void signUp() {
        string newUsername, newPassword;
        bool isAdmin;

        cout << "Podaj nową nazwę użytkownika: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, newUsername);

        for (const auto& creds : credsInfo) {
            if (creds.username == newUsername) {
                cout << "Nazwa użytkownika jest już zajęta!" << endl;
                return;
            }
        }

        cout << "Podaj hasło (min. 6 znaków): ";
        getline(cin, newPassword);

        if (newPassword.length() < 6) {
            cout << "Hasło musi mieć co najmniej 6 znaków." << endl;
            return;
        }

        cout << "Czy konto ma być administracyjne? (1 = Tak, 0 = Nie): ";
        cin >> isAdmin;

        int newUID = credsInfo.size() + 1;
        credsInfo.emplace_back(newUID, newUsername, newPassword, isAdmin);

        fstream output ("creds.csv", ios::app);
            output << newUID << ","<< newUsername << "," << newPassword << ","<< isAdmin << endl;
        cout << "Konto zostało utworzone pomyślnie!" << endl;
    }

    bool isAdmin() const {
        return currentUser && currentUser->isAdmin;
    }

    bool isLoggedIn() const {
        return currentUser != nullptr;
    }

    void logout() {
        currentUser = nullptr;
        cout << "Wylogowano pomyślnie!" << endl;
    }
};

class Book {
public:
    int id;
    string title, genre;
    int popularity;
    bool isAvailable;

    Book(int id, string title, string genre, int popularity, bool isAvailable)
        : id(id), title(title), genre(genre), popularity(popularity), isAvailable(isAvailable) {}

    void displayInfo() const {
        cout << "ID: " << id << ", Tytuł: " << title << ", Gatunek: " << genre
             << ", Popularność: " << popularity
             << ", Dostępność: " << (isAvailable ? "Dostępna" : "Niedostępna") << endl;
    }

    bool borrow() {
        if (isAvailable) {
            isAvailable = false;
            return true;
        } else {
            cout << "Książka \"" << title << "\" jest już wypożyczona." << endl;
            return false;
        }
    }

    bool returnBook() {
        if (!isAvailable) {
            isAvailable = true;
            cout << "Książka \"" << title << "\" została zwrócona." << endl;
            return true;
        } else {
            cout << "Książka \"" << title << "\" jest już dostępna." << endl;
            return false;
        }
    }
};

class Library {
public:
    vector<Book> books;

    void getBooks() {
        fstream libraryFile("books.csv");
        if (!libraryFile.is_open()) {
            cerr << "Error: Unable to open books.csv" << endl;
            return;
        }

        string line;
        while (getline(libraryFile, line)) {
            int id, popularity;
            string title, genre, temp;
            bool isAvailable;

            stringstream inputString(line);
            getline(inputString, temp, ',');
            id = atoi(temp.c_str());
            getline(inputString, title, ',');
            getline(inputString, genre, ',');
            getline(inputString, temp, ',');
            popularity = atoi(temp.c_str());
            getline(inputString, temp, ',');
            istringstream(temp) >> isAvailable;

            books.emplace_back(id, title, genre, popularity, isAvailable);
        }
        libraryFile.close();
    }

    void addBook(const LogIn& login) {
        if (!login.isAdmin()) {
            cout << "Tylko administrator może dodawać książki." << endl;
            return;
        }
        string title, genre;
        int popularity;
        int id = books.back().id + 1;

        cout << "Podaj tytuł książki: ";
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, title);
        cout << "Podaj gatunek książki: ";
        getline(cin, genre);
        cout << "Podaj popularność książki (1-10): ";
        cin >> popularity;

        books.emplace_back(id, title, genre, popularity, true);
        ofstream temp("temp.csv");
        int dlugosc = books.size();
        for(int i = 0; i<dlugosc; i++){
            temp << books[i].id << "," /*<< "\""*/<< books[i].title << ","  << books[i].genre << "," << books[i].popularity << "," << 1 << endl;
        }
        temp.close();
        ifstream temp1("temp.csv");
        cin.ignore();
        string line;
        fstream ksiazki; ksiazki.open("books.csv", ios::out);
        while(getline(temp1,line)){
            ksiazki << line << endl;
        }
        cout << "Książka została pomyślnie dodana!" << endl;
        ksiazki.close();
        temp1.close();
        remove("temp.csv");
    }

    void removeBook(const LogIn& login) {
        if (!login.isAdmin()) {
        cout << "Tylko administrator może usuwać książki." << endl;
        return;
    }

        int bookID;
        cout << "Podaj ID książki do usunięcia: ";
        cin >> bookID;

        ifstream inputFile("books.csv");
        if (!inputFile.is_open()) {
            cerr << "Błąd: Nie można otworzyć pliku books.csv." << endl;
            return;
        }
        auto it = find_if(books.begin(), books.end(), [bookID](const Book& book) {
            return book.id == bookID;
        });

        if (it != books.end()) {
            cout << "Usuwanie książki: " << it->title << endl;
            books.erase(it);

            ofstream outputFile("books.csv");
            if (!outputFile.is_open()) {
                cerr << "Błąd: Nie można otworzyć pliku books.csv do zapisu." << endl;
                return;
            }

            for (const auto& book : books) {
                outputFile << book.id << "," << book.title << "," << book.genre << "," << book.popularity << endl;
            }
            outputFile.close();

            cout << "Książka została pomyślnie usunięta." << endl;
        } else {
            cout << "Nie znaleziono książki o podanym ID." << endl;
        }
    }

    void searchBooksByGenre() {
        string genre;
        cout << "Podaj gatunek książki: ";
        // cin.ignore();
        getline(cin, genre);

        bool found = false;
        for (const auto& book : books) {
            if (book.genre == genre) {
                book.displayInfo();
                found = true;
            }
        }

        if (!found) {
            cout << "Nie znaleziono książek o podanym gatunku." << endl;
        }
    }

    void searchBooksByPopularity() {
        int popularity;
        cout << "Podaj popularność książki (1-10): ";
        cin >> popularity;

        bool found = false;
        for (const auto& book : books) {
            if (book.popularity == popularity) {
                book.displayInfo();
                found = true;
            }
        }

        if (!found) {
            cout << "Nie znaleziono książek o podanej popularności." << endl;
        }
    }
    
    void searchBookByTitle() const {
        string title;
        cin.ignore();
        cout << "Podaj tytuł książki: ";
        getline(cin, title);

        for (const auto& book : books) {
            if (book.title == title) {
                book.displayInfo();
                return;
            }
        }

        cout << "Nie znaleziono książki o podanym tytule!" << endl;
    }

    void showAvailableBooks() {
        for (const auto& book : books) {
            if (book.isAvailable) {
                book.displayInfo();
            }
        }
    }

    void borrowBook() {
            int bookID;
            cout << "Podaj ID książki do wypożyczenia: ";
            cin >> bookID;


            for (auto& book : books) {
            if (book.id == bookID && book.isAvailable) {
                book.isAvailable = false;
                cout << "Książka została wypożyczona!" << endl;

                ofstream outputFile("books.csv");
                if (!outputFile.is_open()) {
                    cerr << "Błąd: Nie można otworzyć pliku books.csv do zapisu." << endl;
                    return;
                }

                for (const auto& book : books) {
                    outputFile << book.id << "," << book.title << "," << book.genre << "," 
                            << book.popularity << "," << (book.isAvailable ? "1" : "0") << endl;
                }
                outputFile.close();
                return;
            }
        }

        cout << "Książka jest niedostępna!" << endl;
    }

    void returnBook() {
            int bookID;
            cout << "Podaj ID książki do zwrócenia: ";
            cin >> bookID;

            for (auto& book : books) {
            if (book.id == bookID && !book.isAvailable) {
                book.isAvailable = true;
                cout << "Książka została zwrócona!" << endl;

                ofstream outputFile("books.csv");
                if (!outputFile.is_open()) {
                    cerr << "Błąd: Nie można otworzyć pliku books.csv do zapisu." << endl;
                    return;
                }

                for (const auto& book : books) {
                    outputFile << book.id << "," << book.title << "," << book.genre << ","
                            << book.popularity << "," << (book.isAvailable ? "1" : "0") << endl;
                }
                outputFile.close();
                return;
            }
        }

        cout << "Książka jest już dostępna lub nie istnieje!" << endl;
    }
};

class User {
public:
    string name;
    Library& library;

    User(string name, Library& library) : name(name), library(library) {}

    void borrowBook() { library.borrowBook(); }
    void returnBook() { library.returnBook(); }
    void searchBookByTitle() { library.searchBookByTitle(); }
    void searchBookByGenre() { library.searchBooksByGenre(); }
    void searchBookByPopularity() { library.searchBooksByPopularity(); }
};

int main() {
    LogIn loginSystem;
    loginSystem.getCreds();

    Library library;
    library.getBooks();

    int choice;

    do {
        cout <<"\033[32m"<<"\n--- MENU ---" <<"\033[0m"<< endl;
        cout << "1. Zaloguj się" << endl;
        cout << "2. Zarejestruj się" << endl;
        cout <<"\033[31m"<<"0. Wyjdź" <<"\033[0m"<< endl;
        cout << "Wybór: ";
        cin >> choice;

        if (choice == 1) {
            loginSystem.performLogin();
            if (loginSystem.isLoggedIn()) {
                User user(loginSystem.currentUser->username, library);

                int userChoice;
                do {
                    cout <<"\033[32m"<< "\n--- Menu Biblioteki ---" <<"\033[0m"<< endl;
                    cout << "1. Pokaż dostępne książki" << endl;
                    cout << "2. Wypożycz książkę" << endl;
                    cout << "3. Zwróć książkę" << endl;
                    cout << "4. Wyszukaj książkę po tytule" << endl;
                    cout << "5. Wyszukaj ksiązke po popularności" << endl;
                    cout << "6. Wyszukaj ksiązkę za gatunkie" << endl;
                    if (loginSystem.isAdmin()) {
                        cout <<"\033[35m"<< "7. Dodaj książkę" <<"\033[0m"<< endl;
                        cout <<"\033[35m"<< "8. Usuń książkę" <<"\033[0m"<< endl;
                    }
                    cout <<"\033[31m"<< "0. Wyloguj się" <<"\033[0m"<< endl;
                    cout << "Wybór: ";
                    cin >> userChoice;

                    switch (userChoice) {
                        case 1:
                            system("clear");
                            library.showAvailableBooks();
                            break;
                        case 2:
                            system("clear");
                            user.borrowBook();
                            break;
                        case 3:
                            system("clear");
                            user.returnBook();
                            break;
                        case 4:
                            system("clear");
                            user.searchBookByTitle();
                            break;
                        case 5:
                            system("clear");
                            user.searchBookByPopularity();
                        case 6:
                            system("clear");
                            user.searchBookByGenre();
                        
                        case 7:
                            if (loginSystem.isAdmin()) {
                                system("clear");
                                library.addBook(loginSystem);
                            }
                            break;
                        case 8:
                            if(loginSystem.isAdmin()){
                                system("clear");
                                library.removeBook(loginSystem);
                            }
                        case 0:
                            loginSystem.logout();
                            break;
                        default:
                            cout << "Nieprawidłowy wybór!" << endl;
                    }
                } while (userChoice != 0);
            }
        } else if (choice == 2) {
            loginSystem.signUp();
        }
    } while (choice != 0);

    cout << "Do widzenia!" << endl;
    return 0;

    return 0;
}
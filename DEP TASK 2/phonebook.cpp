#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

class Contact {
public:
    int id;
    string name;
    string phoneNumber;

    Contact(int id, string name, string phoneNumber) : id(id), name(name), phoneNumber(phoneNumber) {}
};

class ContactManager {
private:
    vector<Contact> contacts;
    const string fileName = "contacts.txt";

    void saveContacts() {
        ofstream file(fileName, ofstream::trunc);
        if (!file.is_open()) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        for (const auto& contact : contacts) {
            file << contact.id << endl;
            file << contact.name << endl;
            file << contact.phoneNumber << endl;
        }
        file.close();
    }

    bool isUniqueId(int id) {
        return find_if(contacts.begin(), contacts.end(), [id](const Contact& c) { return c.id == id; }) == contacts.end();
    }

    int getValidatedId() {
        int id;
        while (true) {
            cout << "Enter contact ID: ";
            cin >> id;
            if (cin.fail() || !isUniqueId(id)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid or duplicate ID. Please enter a unique numeric ID.\n";
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }
        return id;
    }

public:
    ContactManager() {
        loadContacts();
    }

    void loadContacts() {
        ifstream file(fileName);
        if (!file.is_open()) {
            cerr << "Error opening file for reading.\n";
            return;
        }

        int id;
        string name;
        string phoneNumber;
        while (file >> id >> ws && getline(file, name) && getline(file, phoneNumber)) {
            contacts.emplace_back(id, name, phoneNumber);
        }
        file.close();
    }

    void addContact() {
        int id = getValidatedId();
        string name, phoneNumber;

        cout << "Enter contact name: ";
        getline(cin, name);
        cout << "Enter contact phone number: ";
        getline(cin, phoneNumber);

        contacts.emplace_back(id, name, phoneNumber);
        saveContacts();
        cout << "Contact added successfully.";
        pressAnyKeyToContinue();
    }

    void viewContacts() {
        if (contacts.empty()) {
            cout << "No contacts available.\n";
        } else {
            cout << setw(5) << "ID" << setw(20) << "Name" << setw(20) << "Phone Number" << endl;
            for (const auto& contact : contacts) {
                cout << setw(5) << contact.id << setw(20) << contact.name << setw(20) << contact.phoneNumber << endl;
            }
        }
        pressAnyKeyToContinue();
    }

    void deleteContact() {
        int id;
        cout << "Enter contact ID to delete: ";
        cin >> id;

        auto it = remove_if(contacts.begin(), contacts.end(), [id](const Contact& c) { return c.id == id; });
        if (it != contacts.end()) {
            contacts.erase(it, contacts.end());
            saveContacts();
            cout << "Contact deleted successfully.";
        } else {
            cout << "Contact not found.";
        }
        pressAnyKeyToContinue();
    }

    void editContact() {
        int id;
        cout << "Enter contact ID to edit: ";
        cin >> id;
        cin.ignore();

        for (auto& contact : contacts) {
            if (contact.id == id) {
                string newName, newPhoneNumber;
                cout << "Enter new name: ";
                getline(cin, newName);
                cout << "Enter new phone number: ";
                getline(cin, newPhoneNumber);

                contact.name = newName;
                contact.phoneNumber = newPhoneNumber;
                saveContacts();
                cout << "Contact updated successfully.";
                pressAnyKeyToContinue();
                return;
            }
        }
        cout << "Contact not found.";
        pressAnyKeyToContinue();
    }

    void searchContact() {
        cout << "Enter ID, name, or phone number to search: ";
        cin.ignore();
        string query;
        getline(cin, query);

        bool found = false;
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(20) << "Phone Number" << endl;
        for (const auto& contact : contacts) {
            if (to_string(contact.id) == query || contact.name.find(query) != string::npos || contact.phoneNumber.find(query) != string::npos) {
                found = true;
                cout << setw(5) << contact.id << setw(20) << contact.name << setw(20) << contact.phoneNumber << endl;
            }
        }

        if (!found) {
            cout << "No contacts found matching the query.";
        }
        pressAnyKeyToContinue();
    }

    void pressAnyKeyToContinue() {
        cout << "\nPress any key to continue...";
        cin.ignore();
        cin.get();
        #if defined(_WIN32) || defined(_WIN64)
            system("CLS");
        #else
            system("clear");
        #endif
    }
};

int main() {
    ContactManager manager;
    int choice;

    do {
        #if defined(_WIN32) || defined(_WIN64)
            system("CLS");
        #else
            system("clear");
        #endif
        cout << "\nContact Management System\n";
        cout << "1. Add Contact\n";
        cout << "2. View Contacts\n";
        cout << "3. Delete Contact\n";
        cout << "4. Edit Contact\n";
        cout << "5. Search Contact\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addContact();
                break;
            case 2:
                manager.viewContacts();
                break;
            case 3:
                manager.deleteContact();
                break;
            case 4:
                manager.editContact();
                break;
            case 5:
                manager.searchContact();
                break;
            case 6:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.";
                manager.pressAnyKeyToContinue();
        }
    } while (choice != 6);

    return 0;
}

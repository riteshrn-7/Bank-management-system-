#include <iostream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

class Customer {
protected:
    int customerId;
    string name;
    string mobile;
    string address;
    int age;
    string email;

public:
    Customer() {
        customerId = 0;
        name = "";
        mobile = "";
        address = "";
        age = 0;
        email = "";
    }

    Customer(int id, string n, string m, string a, int ag, string e) {
        customerId = id;
        name = n;
        mobile = m;
        address = a;
        age = ag;
        email = e;
    }

    void createCustomer() {
        cout << "\n===== Create Customer =====\n";
        cout << "Enter Customer ID: ";
        cin >> customerId;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Enter Mobile Number: ";
        getline(cin, mobile);
        cout << "Enter Address: ";
        getline(cin, address);
        cout << "Enter Age: ";
        cin >> age;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Email: ";
        getline(cin, email);
        cout << "\nCustomer created successfully!\n";
    }

    void displayCustomer() const {
        cout << "\n===== Customer Details =====\n";
        cout << "Customer ID : " << customerId << "\n";
        cout << "Name        : " << name << "\n";
        cout << "Mobile      : " << mobile << "\n";
        cout << "Address     : " << address << "\n";
        cout << "Age         : " << age << "\n";
        cout << "Email       : " << email << "\n";
    }

    void updateSpecificDetails() {
        int choice;
        do {
            cout << "\n--- Select Field to Update ---\n";
            cout << "1. Name\n";
            cout << "2. Mobile Number\n";
            cout << "3. Address\n";
            cout << "4. Age\n";
            cout << "5. Email\n";
            cout << "6. Update All Details\n";
            cout << "7. Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case 1:
                cout << "Enter New Name: ";
                getline(cin, name);
                cout << "Name updated successfully!\n";
                break;
            case 2:
                cout << "Enter New Mobile Number: ";
                getline(cin, mobile);
                cout << "Mobile updated successfully!\n";
                break;
            case 3:
                cout << "Enter New Address: ";
                getline(cin, address);
                cout << "Address updated successfully!\n";
                break;
            case 4:
                cout << "Enter New Age: ";
                cin >> age;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Age updated successfully!\n";
                break;
            case 5:
                cout << "Enter New Email: ";
                getline(cin, email);
                cout << "Email updated successfully!\n";
                break;
            case 6:
                cout << "Enter New Name: ";
                getline(cin, name);
                cout << "Enter New Mobile Number: ";
                getline(cin, mobile);
                cout << "Enter New Address: ";
                getline(cin, address);
                cout << "Enter New Age: ";
                cin >> age;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter New Email: ";
                getline(cin, email);
                cout << "All details updated successfully!\n";
                break;
            case 7:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice! Please select 1-7.\n";
            }
        } while (choice != 7);
    }

    int getCustomerId() const { return customerId; }
    string getName() const { return name; }
    string getMobile() const { return mobile; }
    string getAddress() const { return address; }
    int getAge() const { return age; }
    string getEmail() const { return email; }

    void setCustomerId(int id) { customerId = id; }
    void setName(string n) { name = n; }
    void setMobile(string m) { mobile = m; }
    void setAddress(string a) { address = a; }
    void setAge(int ag) { age = ag; }
    void setEmail(string e) { email = e; }
};

int main() {
    vector<Customer> customers;
    int choice;

    do {
        cout << "\n===== CUSTOMER MANAGEMENT =====\n";
        cout << "1. Create Customer\n";
        cout << "2. Display Customer by ID\n";
        cout << "3. Display All Customers\n";
        cout << "4. Update Customer\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            Customer newCustomer;
            newCustomer.createCustomer();
            customers.push_back(newCustomer);
            break;
        }
        case 2: {
            if (customers.empty()) {
                cout << "No customers found.\n";
                break;
            }
            int searchId;
            cout << "Enter Customer ID to display: ";
            cin >> searchId;
            bool found = false;
            for (const auto& c : customers) {
                if (c.getCustomerId() == searchId) {
                    c.displayCustomer();
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Customer ID not found.\n";
            break;
        }
        case 3: {
            if (customers.empty()) {
                cout << "No customers found.\n";
                break;
            }
            for (const auto& c : customers) {
                c.displayCustomer();
            }
            break;
        }
        case 4: {
            if (customers.empty()) {
                cout << "No customers found to update.\n";
                break;
            }
            int searchId;
            cout << "Enter Customer ID to update: ";
            cin >> searchId;
            bool found = false;
            for (auto& c : customers) {
                if (c.getCustomerId() == searchId) {
                    c.updateSpecificDetails();
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Customer ID not found.\n";
            break;
        }
        case 5:
            cout << "Exiting program...\n";
            break;
        default:
            cout << "Invalid choice! Please select 1-5.\n";
        }
    } while (choice != 5);

    return 0;
}
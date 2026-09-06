#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

// Represents a verified bank client
class Customer {
private:
    int customerId;
    string name;
    string mobile;
    string address;
    int age;
    string email;

public:
    Customer() : customerId(0), name(""), mobile(""), address(""), age(0), email("") {}

    Customer(int id, string n, string m, string a, int ag, string e)
        : customerId(id), name(n), mobile(m), address(a), age(ag), email(e) {}

    int getCustomerId() const { return customerId; }
    string getName() const { return name; }
    string getMobile() const { return mobile; }
    string getAddress() const { return address; }
    int getAge() const { return age; }
    string getEmail() const { return email; }

    void setName(const string& n) { name = n; }
    void setMobile(const string& m) { mobile = m; }
    void setAddress(const string& a) { address = a; }
    void setAge(int ag) { age = ag; }
    void setEmail(const string& e) { email = e; }

    void displayProfile() const {
        cout << "\n--- Customer Profile ---\n";
        cout << "Customer ID : " << customerId << "\n";
        cout << "Name        : " << name << "\n";
        cout << "Mobile      : " << mobile << "\n";
        cout << "Address     : " << address << "\n";
        cout << "Age         : " << age << "\n";
        cout << "Email       : " << email << "\n";
    }
};

// Represents a financial ledger linked to a Customer
class Account {
private:
    int accountNumber;
    int customerId;
    double balance;

public:
    Account(int accNum, int custId, double initialDeposit)
        : accountNumber(accNum), customerId(custId), balance(initialDeposit) {}

    int getAccountNumber() const { return accountNumber; }
    int getCustomerId() const { return customerId; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        if (amount <= 0) {
            cout << "[Error] Deposit amount must be greater than zero.\n";
            return;
        }
        balance += amount;
        cout << "[Success] Deposited $" << fixed << setprecision(2) << amount 
             << ". Updated balance: $" << balance << "\n";
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "[Error] Withdrawal amount must be greater than zero.\n";
            return false;
        }
        if (amount > balance) {
            cout << "[Error] Insufficient funds. Available balance: $" 
                 << fixed << setprecision(2) << balance << "\n";
            return false;
        }
        balance -= amount;
        cout << "[Success] Withdrew $" << fixed << setprecision(2) << amount 
             << ". Remaining balance: $" << balance << "\n";
        return true;
    }
};

// Orchestration engine managing Customers and Accounts
class BankSystem {
private:
    vector<Customer> customers;
    vector<Account> accounts;
    int nextCustomerId = 5001;
    int nextAccountNumber = 10001;

    void clearBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Customer* findCustomer(int id) {
        for (auto& c : customers) {
            if (c.getCustomerId() == id) return &c;
        }
        return nullptr;
    }

    Account* findAccount(int accNum) {
        for (auto& a : accounts) {
            if (a.getAccountNumber() == accNum) return &a;
        }
        return nullptr;
    }

public:
    void onboardCustomer() {
        string name, mobile, address, email;
        int age;
        double initialDeposit;

        cout << "\n========================================\n";
        cout << "     CUSTOMER ONBOARDING & ACCOUNT      \n";
        cout << "========================================\n";

        cout << "Enter Full Name: ";
        clearBuffer();
        getline(cin, name);

        cout << "Enter Mobile Number: ";
        getline(cin, mobile);

        cout << "Enter Address: ";
        getline(cin, address);

        cout << "Enter Age: ";
        while (!(cin >> age) || age < 18) {
            cout << "Invalid age. Must be 18 or older to open an account: ";
            clearBuffer();
        }

        cout << "Enter Email: ";
        clearBuffer();
        getline(cin, email);

        cout << "Enter Initial Opening Deposit: $";
        while (!(cin >> initialDeposit) || initialDeposit < 0) {
            cout << "Invalid amount. Enter a valid positive number: $";
            clearBuffer();
        }

        int currentCustId = nextCustomerId++;
        int currentAccNum = nextAccountNumber++;

        customers.emplace_back(currentCustId, name, mobile, address, age, email);
        accounts.emplace_back(currentAccNum, currentCustId, initialDeposit);

        cout << "\nAccount successfully created!\n";
        cout << ">> Assigned Customer ID : " << currentCustId << "\n";
        cout << ">> Assigned Account No. : " << currentAccNum << "\n";
    }

    void displayFullAccount() {
        int accNum;
        cout << "\nEnter Account Number to View: ";
        if (!(cin >> accNum)) {
            cout << "[Error] Invalid input.\n";
            clearBuffer();
            return;
        }

        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "[Error] Account number not found.\n";
            return;
        }

        Customer* cust = findCustomer(acc->getCustomerId());
        if (!cust) {
            cout << "[Error] Linked customer profile missing.\n";
            return;
        }

        cout << "\n========================================\n";
        cout << "          BANK ACCOUNT STATEMENT        \n";
        cout << "========================================\n";
        cout << "Account Number : " << acc->getAccountNumber() << "\n";
        cout << "Current Balance: $" << fixed << setprecision(2) << acc->getBalance() << "\n";
        cust->displayProfile();
        cout << "========================================\n";
    }

    void updateCustomerProfile() {
        int custId;
        cout << "\nEnter Customer ID to Update: ";
        if (!(cin >> custId)) {
            cout << "[Error] Invalid input.\n";
            clearBuffer();
            return;
        }

        Customer* cust = findCustomer(custId);
        if (!cust) {
            cout << "[Error] Customer ID not found.\n";
            return;
        }

        int updateChoice = 0;
        do {
            cout << "\n--- Update Details for " << cust->getName() << " ---\n";
            cout << "1. Mobile Number\n";
            cout << "2. Address\n";
            cout << "3. Email\n";
            cout << "4. Done Updating\n";
            cout << "Enter selection (1-4): ";
            if (!(cin >> updateChoice)) {
                clearBuffer();
                continue;
            }

            clearBuffer();
            switch (updateChoice) {
                case 1: {
                    string newMobile;
                    cout << "Enter New Mobile: ";
                    getline(cin, newMobile);
                    cust->setMobile(newMobile);
                    cout << "[Updated] Mobile saved.\n";
                    break;
                }
                case 2: {
                    string newAddr;
                    cout << "Enter New Address: ";
                    getline(cin, newAddr);
                    cust->setAddress(newAddr);
                    cout << "[Updated] Address saved.\n";
                    break;
                }
                case 3: {
                    string newEmail;
                    cout << "Enter New Email: ";
                    getline(cin, newEmail);
                    cust->setEmail(newEmail);
                    cout << "[Updated] Email saved.\n";
                    break;
                }
                case 4:
                    cout << "Profile update completed.\n";
                    break;
                default:
                    cout << "Invalid choice. Select between 1 and 4.\n";
            }
        } while (updateChoice != 4);
    }

    void depositFunds() {
        int accNum;
        double amount;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) {
            clearBuffer();
            return;
        }

        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "[Error] Account not found.\n";
            return;
        }

        cout << "Enter Deposit Amount: $";
        if (!(cin >> amount)) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        acc->deposit(amount);
    }

    void withdrawFunds() {
        int accNum;
        double amount;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) {
            clearBuffer();
            return;
        }

        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "[Error] Account not found.\n";
            return;
        }

        cout << "Enter Withdrawal Amount: $";
        if (!(cin >> amount)) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        acc->withdraw(amount);
    }

    void checkBalance() {
        int accNum;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) {
            clearBuffer();
            return;
        }

        Account* acc = findAccount(accNum);
        if (!acc) {
            cout << "[Error] Account not found.\n";
            return;
        }

        cout << "\n>> Account No: " << acc->getAccountNumber() << "\n";
        cout << ">> Available Balance: $" << fixed << setprecision(2) << acc->getBalance() << "\n";
    }
};

int main() {
    BankSystem system;
    int choice = 0;

    while (choice != 7) {
        cout << "\n====================================\n";
        cout << "       ENTERPRISE BANK SYSTEM       \n";
        cout << "====================================\n";
        cout << "1. Open Account (Create Customer)\n";
        cout << "2. View Account & Customer Details\n";
        cout << "3. Update Customer Details\n";
        cout << "4. Deposit Money\n";
        cout << "5. Withdraw Money\n";
        cout << "6. Check Balance\n";
        cout << "7. Exit\n";
        cout << "Select an option (1-7): ";

        if (!(cin >> choice)) {
            cout << "Invalid selection. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                system.onboardCustomer();
                break;
            case 2:
                system.displayFullAccount();
                break;
            case 3:
                system.updateCustomerProfile();
                break;
            case 4:
                system.depositFunds();
                break;
            case 5:
                system.withdrawFunds();
                break;
            case 6:
                system.checkBalance();
                break;
            case 7:
                cout << "Shutting down system. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please select from 1 to 7.\n";
                break;
        }
    }

    return 0;
}
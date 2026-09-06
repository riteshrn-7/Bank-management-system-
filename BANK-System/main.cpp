#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <functional>

using namespace std;

// Helper to get formatted current timestamp
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t in_time_t = chrono::system_clock::to_time_t(now);
    tm buf;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&buf, &in_time_t);
#else
    localtime_r(&in_time_t, &buf);
#endif
    char str[20];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &buf);
    return string(str);
}

// 1. TRANSACTION MODEL
struct Transaction {
    int accountNumber;
    string timestamp;
    string type;
    double amount;
    double balanceAfter;

    string serialize() const {
        return to_string(accountNumber) + "|" + timestamp + "|" + type + "|" + to_string(amount) + "|" + to_string(balanceAfter);
    }

    static Transaction deserialize(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() == 5) {
            try {
                return {stoi(tokens[0]), tokens[1], tokens[2], stod(tokens[3]), stod(tokens[4])};
            } catch (...) {
                return {0, "", "", 0.0, 0.0};
            }
        }
        return {0, "", "", 0.0, 0.0};
    }
};

// 2. CUSTOMER MODEL
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

    void setMobile(const string& m) { mobile = m; }
    void setAddress(const string& a) { address = a; }
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

    string serialize() const {
        return to_string(customerId) + "|" + name + "|" + mobile + "|" + address + "|" + to_string(age) + "|" + email;
    }

    static Customer deserialize(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() == 6) {
            try {
                return Customer(stoi(tokens[0]), tokens[1], tokens[2], tokens[3], stoi(tokens[4]), tokens[5]);
            } catch (...) {
                return Customer();
            }
        }
        return Customer();
    }
};

// 3. ACCOUNT BASE MODEL
class Account {
protected:
    int accountNumber;
    int customerId;
    double balance;
    string accountType;
    size_t pinHash;

public:
    Account() : accountNumber(0), customerId(0), balance(0.0), accountType("SAVINGS"), pinHash(0) {}

    Account(int accNum, int custId, double initialDeposit, string accType, size_t hashVal)
        : accountNumber(accNum), customerId(custId), balance(initialDeposit), accountType(accType), pinHash(hashVal) {}

    virtual ~Account() {}

    int getAccountNumber() const { return accountNumber; }
    int getCustomerId() const { return customerId; }
    double getBalance() const { return balance; }
    string getAccountType() const { return accountType; }

    bool verifyPin(const string& enteredPin) const {
        hash<string> hasher;
        return hasher(enteredPin) == pinHash;
    }

    void deposit(double amount) {
        balance += amount;
    }

    virtual bool canWithdraw(double amount) const = 0;
    virtual bool withdraw(double amount, vector<Transaction>& txLog) = 0;
    virtual void applyMonthlyCharges(vector<Transaction>& txLog) = 0;

    virtual string serialize() const {
        return to_string(accountNumber) + "|" + to_string(customerId) + "|" + to_string(balance) + "|" + accountType + "|" + to_string(pinHash);
    }

    static Account* deserialize(const string& line);
};

// 4. SAVINGS ACCOUNT
class SavingsAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 500.0;
    static constexpr double LOW_BAL_FEE = 25.0;
    static constexpr double WITHDRAW_LIMIT = 20000.0;

    SavingsAccount(int accNum, int custId, double initialDeposit, size_t hashVal)
        : Account(accNum, custId, initialDeposit, "SAVINGS", hashVal) {}

    bool canWithdraw(double amount) const override {
        if (amount > WITHDRAW_LIMIT) return false;
        if (amount > balance) return false;
        return true;
    }

    bool withdraw(double amount, vector<Transaction>& txLog) override {
        if (amount > WITHDRAW_LIMIT) {
            cout << "[Error] Exceeds single withdrawal limit of Rs " << fixed << setprecision(2) << WITHDRAW_LIMIT << "\n";
            return false;
        }
        if (amount > balance) {
            cout << "[Error] Insufficient balance. Available: Rs " << fixed << setprecision(2) << balance << "\n";
            return false;
        }

        balance -= amount;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "WITHDRAWAL", amount, balance});
        cout << "[Success] Withdrew Rs " << fixed << setprecision(2) << amount << ". Remaining: Rs " << balance << "\n";

        if (balance < MIN_BALANCE) {
            balance -= LOW_BAL_FEE;
            txLog.push_back({accountNumber, getCurrentTimestamp(), "MIN_BAL_FEE", LOW_BAL_FEE, balance});
            cout << "[Notice] Balance fell below Rs " << fixed << setprecision(2) << MIN_BALANCE 
                 << ". Charged penalty of Rs " << LOW_BAL_FEE << ". Final Balance: Rs " << balance << "\n";
        }
        return true;
    }

    void applyMonthlyCharges(vector<Transaction>& txLog) override {
        if (balance < MIN_BALANCE) {
            balance -= LOW_BAL_FEE;
            txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", LOW_BAL_FEE, balance});
            cout << ">> Acc " << accountNumber << " (Savings): Debited Rs " << LOW_BAL_FEE << " for low balance.\n";
        }
    }
};

// 5. CURRENT ACCOUNT
class CurrentAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 10000.0;
    static constexpr double MONTHLY_PENALTY = 500.0;
    static constexpr double OVERDRAFT_LIMIT = 25000.0;

    CurrentAccount(int accNum, int custId, double initialDeposit, size_t hashVal)
        : Account(accNum, custId, initialDeposit, "CURRENT", hashVal) {}

    bool canWithdraw(double amount) const override {
        if ((balance - amount) < -OVERDRAFT_LIMIT) return false;
        return true;
    }

    bool withdraw(double amount, vector<Transaction>& txLog) override {
        if ((balance - amount) < -OVERDRAFT_LIMIT) {
            cout << "[Error] Overdraft limit exceeded! Maximum allowed credit is Rs -" 
                 << fixed << setprecision(2) << OVERDRAFT_LIMIT << "\n";
            return false;
        }

        balance -= amount;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "WITHDRAWAL", amount, balance});
        cout << "[Success] Withdrew Rs " << fixed << setprecision(2) << amount << ". Remaining: Rs " << balance << "\n";

        if (balance < MIN_BALANCE) {
            cout << "[Notice] Current Account balance is under minimum requirement of Rs 10000.00.\n";
        }
        return true;
    }

    void applyMonthlyCharges(vector<Transaction>& txLog) override {
        if (balance < MIN_BALANCE) {
            balance -= MONTHLY_PENALTY;
            txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", MONTHLY_PENALTY, balance});
            cout << ">> Acc " << accountNumber << " (Current): Below Rs 10,000 threshold. Debited Rs " 
                 << MONTHLY_PENALTY << " penalty fee.\n";
        }
    }
};

Account* Account::deserialize(const string& line) {
    stringstream ss(line);
    string item;
    vector<string> tokens;
    while (getline(ss, item, '|')) {
        tokens.push_back(item);
    }
    if (tokens.size() == 5) {
        try {
            int accNum = stoi(tokens[0]);
            int custId = stoi(tokens[1]);
            double bal = stod(tokens[2]);
            string type = tokens[3];
            size_t hVal = stoull(tokens[4]);

            if (type == "CURRENT") {
                return new CurrentAccount(accNum, custId, bal, hVal);
            } else {
                return new SavingsAccount(accNum, custId, bal, hVal);
            }
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// 6. BANK SYSTEM CONTROLLER
class BankSystem {
private:
    vector<Customer> customers;
    vector<Account*> accounts;
    vector<Transaction> transactions;

    const string customerFile = "customers.txt";
    const string accountFile = "accounts.txt";
    const string transactionFile = "transactions.txt";

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
        for (auto* a : accounts) {
            if (a->getAccountNumber() == accNum) return a;
        }
        return nullptr;
    }

    bool authenticate(Account* acc) {
        string pin;
        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter 4-Digit PIN (" << attempts << " attempts left): ";
            cin >> pin;
            if (acc->verifyPin(pin)) {
                return true;
            }
            attempts--;
            cout << "[Error] Incorrect PIN.\n";
        }
        cout << "[Access Denied] Maximum authentication attempts reached.\n";
        return false;
    }

    void loadData() {
        customers.clear();
        for (auto* a : accounts) delete a;
        accounts.clear();
        transactions.clear();

        ifstream cIn(customerFile);
        string line;
        while (getline(cIn, line)) {
            if (!line.empty()) customers.push_back(Customer::deserialize(line));
        }
        cIn.close();

        ifstream aIn(accountFile);
        while (getline(aIn, line)) {
            if (!line.empty()) {
                Account* acc = Account::deserialize(line);
                if (acc) accounts.push_back(acc);
            }
        }
        aIn.close();

        ifstream tIn(transactionFile);
        while (getline(tIn, line)) {
            if (!line.empty()) transactions.push_back(Transaction::deserialize(line));
        }
        tIn.close();
    }

    void saveData() {
        ofstream cOut(customerFile);
        for (const auto& c : customers) {
            cOut << c.serialize() << "\n";
        }
        cOut.close();

        ofstream aOut(accountFile);
        for (const auto* a : accounts) {
            aOut << a->serialize() << "\n";
        }
        aOut.close();

        ofstream tOut(transactionFile);
        for (const auto& t : transactions) {
            tOut << t.serialize() << "\n";
        }
        tOut.close();
    }

public:
    BankSystem() {
        loadData();
    }

    ~BankSystem() {
        for (auto* a : accounts) delete a;
    }

    void onboardCustomer() {
        string name, mobile, address, email, pin;
        int age;
        int accountTypeChoice;
        double initialDeposit;

        cout << "\n===== Customer Registration =====\n";
        cout << "Full Name: ";
        clearBuffer();
        getline(cin, name);

        cout << "Mobile: ";
        getline(cin, mobile);

        cout << "Address: ";
        getline(cin, address);

        cout << "Age: ";
        while (!(cin >> age) || age < 18) {
            cout << "Invalid. Minimum age is 18: ";
            clearBuffer();
        }

        cout << "Email: ";
        clearBuffer();
        getline(cin, email);

        cout << "Set a 4-Digit PIN: ";
        while (true) {
            cin >> pin;
            if (pin.length() == 4 && pin.find_first_not_of("0123456789") == string::npos) {
                break;
            }
            cout << "Invalid PIN. Must be exactly 4 digits (e.g., 1234): ";
            clearBuffer();
        }

        cout << "Select Account Type:\n";
        cout << "  1. Savings Account (Minimum Deposit: Rs 500)\n";
        cout << "  2. Current Account (Minimum Deposit: Rs 10,000 | Low-Balance Monthly Fee: Rs 500)\n";
        cout << "Choice (1-2): ";
        while (!(cin >> accountTypeChoice) || (accountTypeChoice != 1 && accountTypeChoice != 2)) {
            cout << "Invalid. Select 1 or 2: ";
            clearBuffer();
        }

        double requiredDeposit = (accountTypeChoice == 1) ? 500.0 : 10000.0;
        cout << "Initial Opening Deposit: Rs ";
        while (!(cin >> initialDeposit) || initialDeposit < requiredDeposit) {
            cout << "[Error] Minimum initial deposit for this account is Rs " 
                 << fixed << setprecision(2) << requiredDeposit << ". Enter again: Rs ";
            clearBuffer();
        }

        int custId = customers.empty() ? 5001 : customers.back().getCustomerId() + 1;
        int accNum = accounts.empty() ? 10001 : accounts.back()->getAccountNumber() + 1;

        hash<string> hasher;
        size_t pinHash = hasher(pin);

        customers.emplace_back(custId, name, mobile, address, age, email);

        if (accountTypeChoice == 1) {
            accounts.push_back(new SavingsAccount(accNum, custId, initialDeposit, pinHash));
        } else {
            accounts.push_back(new CurrentAccount(accNum, custId, initialDeposit, pinHash));
        }

        transactions.push_back({accNum, getCurrentTimestamp(), "INITIAL_DEP", initialDeposit, initialDeposit});
        saveData();

        cout << "\n[Account Created Successfully]\n";
        cout << "Customer ID  : " << custId << "\n";
        cout << "Account No   : " << accNum << "\n";
        cout << "Account Type : " << (accountTypeChoice == 1 ? "SAVINGS" : "CURRENT") << "\n";
    }

    void displayAccountDetails() {
        int accNum;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        if (!authenticate(acc)) return;

        Customer* cust = findCustomer(acc->getCustomerId());
        if (!cust) { cout << "[Error] Associated customer profile missing.\n"; return; }

        cout << "\n======================================================================\n";
        cout << "                       ACCOUNT SUMMARY STATEMENT                      \n";
        cout << "======================================================================\n";
        cout << "Account No      : " << acc->getAccountNumber() << "\n";
        cout << "Account Type    : " << acc->getAccountType() << "\n";
        cout << "Current Balance : Rs " << fixed << setprecision(2) << acc->getBalance() << "\n";
        cust->displayProfile();

        cout << "\n--- Transaction History ---\n";
        bool hasTx = false;
        cout << left << setw(21) << "Date & Time"
             << setw(20) << "Type" 
             << setw(14) << "Amount (Rs)" 
             << setw(16) << "Balance After" << "\n";
        cout << string(71, '-') << "\n";

        for (const auto& tx : transactions) {
            if (tx.accountNumber == accNum) {
                hasTx = true;
                cout << left << setw(21) << tx.timestamp
                     << setw(20) << tx.type 
                     << setw(14) << fixed << setprecision(2) << tx.amount 
                     << setw(16) << fixed << setprecision(2) << tx.balanceAfter << "\n";
            }
        }
        if (!hasTx) cout << "No transaction records found.\n";
        cout << "======================================================================\n";
    }

    void updateCustomer() {
        int custId;
        cout << "\nEnter Customer ID: ";
        if (!(cin >> custId)) { clearBuffer(); return; }

        Customer* cust = findCustomer(custId);
        if (!cust) { cout << "[Error] Customer ID not found.\n"; return; }

        int choice = 0;
        do {
            cout << "\n--- Update Details (" << cust->getName() << ") ---\n";
            cout << "1. Mobile | 2. Address | 3. Email | 4. Finish & Save\n";
            cout << "Choice: ";
            if (!(cin >> choice)) { clearBuffer(); continue; }
            clearBuffer();

            if (choice == 1) {
                string m; cout << "New Mobile: "; getline(cin, m); cust->setMobile(m);
            } else if (choice == 2) {
                string a; cout << "New Address: "; getline(cin, a); cust->setAddress(a);
            } else if (choice == 3) {
                string e; cout << "New Email: "; getline(cin, e); cust->setEmail(e);
            }
        } while (choice != 4);

        saveData();
    }

    void deposit() {
        int accNum; double amount;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        cout << "Enter Deposit Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid deposit amount.\n";
            clearBuffer();
            return;
        }

        acc->deposit(amount);
        transactions.push_back({accNum, getCurrentTimestamp(), "DEPOSIT", amount, acc->getBalance()});
        saveData();

        cout << "[Success] Deposited Rs " << fixed << setprecision(2) << amount 
             << ". New Balance: Rs " << acc->getBalance() << "\n";
    }

    void withdraw() {
        int accNum; double amount;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        if (!authenticate(acc)) return;

        cout << "Enter Withdrawal Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid withdrawal amount.\n";
            clearBuffer();
            return;
        }

        if (acc->withdraw(amount, transactions)) {
            saveData();
        }
    }

    void checkBalance() {
        int accNum;
        cout << "\nEnter Account Number: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        if (!authenticate(acc)) return;

        cout << "Account No: " << acc->getAccountNumber() 
             << " | Type: " << acc->getAccountType()
             << " | Balance: Rs " << fixed << setprecision(2) << acc->getBalance() << "\n";
    }

    void transferFunds() {
        int senderAccNum, receiverAccNum;
        double amount;

        cout << "\n===== Inter-Account Fund Transfer =====\n";
        cout << "Enter Sender Account Number: ";
        if (!(cin >> senderAccNum)) { clearBuffer(); return; }

        Account* sender = findAccount(senderAccNum);
        if (!sender) {
            cout << "[Error] Sender account not found.\n";
            return;
        }

        if (!authenticate(sender)) return;

        cout << "Enter Receiver Account Number: ";
        if (!(cin >> receiverAccNum)) { clearBuffer(); return; }

        if (senderAccNum == receiverAccNum) {
            cout << "[Error] Sender and Receiver accounts cannot be identical.\n";
            return;
        }

        Account* receiver = findAccount(receiverAccNum);
        if (!receiver) {
            cout << "[Error] Receiver account does not exist.\n";
            return;
        }

        cout << "Enter Transfer Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid transfer amount.\n";
            clearBuffer();
            return;
        }

        // ATOMIC CHECK: Verify if sender can withdraw before touching any balances
        if (!sender->canWithdraw(amount)) {
            cout << "[Transfer Failed] Sender does not have sufficient balance or limit clearance.\n";
            return;
        }

        // ATOMIC EXECUTION: Debit Sender -> Credit Receiver
        string timeNow = getCurrentTimestamp();

        // 1. Debit
        sender->withdraw(amount, transactions);
        // Overwrite the last log type to be specific to transfer
        transactions.back().type = "TRANSFER_TO_" + to_string(receiverAccNum);

        // 2. Credit
        receiver->deposit(amount);
        transactions.push_back({receiverAccNum, timeNow, "TRANSFER_FROM_" + to_string(senderAccNum), amount, receiver->getBalance()});

        // 3. Persist atomically
        saveData();

        cout << "[Success] Rs " << fixed << setprecision(2) << amount 
             << " transferred successfully to Account " << receiverAccNum << "!\n";
    }

    void runMonthlyMaintenanceCycle() {
        cout << "\n[Running Monthly Account Audit & Low-Balance Deduction Cycle...]\n";
        for (auto* acc : accounts) {
            acc->applyMonthlyCharges(transactions);
        }
        saveData();
        cout << "[Audit Complete] Applicable monthly penalties have been processed and logged.\n";
    }
};

int main() {
    BankSystem system;
    int choice = 0;

    while (choice != 9) {
        cout << "\n=========================================\n";
        cout << "          SECURE BANKING PORTAL          \n";
        cout << "=========================================\n";
        cout << "1. Open Account\n";
        cout << "2. View Account & Ledger Statement\n";
        cout << "3. Update Customer Info\n";
        cout << "4. Deposit\n";
        cout << "5. Withdraw\n";
        cout << "6. Quick Balance\n";
        cout << "7. Transfer Funds (Inter-Account)\n";
        cout << "8. Run Monthly Maintenance Cycle (Audit)\n";
        cout << "9. Exit\n";
        cout << "Select (1-9): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.onboardCustomer(); break;
            case 2: system.displayAccountDetails(); break;
            case 3: system.updateCustomer(); break;
            case 4: system.deposit(); break;
            case 5: system.withdraw(); break;
            case 6: system.checkBalance(); break;
            case 7: system.transferFunds(); break;
            case 8: system.runMonthlyMaintenanceCycle(); break;
            case 9: cout << "Exiting system. Goodbye!\n"; break;
            default: cout << "Invalid selection.\n"; break;
        }
    }
    return 0;
}
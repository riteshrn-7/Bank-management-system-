#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <functional>

using namespace std;

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

// 1. APPLICATION MODEL (For account requests awaiting admin approval)
struct AccountApplication {
    int appId;
    int customerId;
    string name;
    string mobile;
    string address;
    int age;
    string email;
    string accountType;
    double initialDeposit;
    size_t pinHash;
    string status; // "PENDING", "APPROVED", "REJECTED"

    string serialize() const {
        return to_string(appId) + "|" + to_string(customerId) + "|" + name + "|" + mobile + "|" + 
               address + "|" + to_string(age) + "|" + email + "|" + accountType + "|" + 
               to_string(initialDeposit) + "|" + to_string(pinHash) + "|" + status;
    }

    static AccountApplication deserialize(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() == 11) {
            try {
                return {stoi(tokens[0]), stoi(tokens[1]), tokens[2], tokens[3], tokens[4],
                        stoi(tokens[5]), tokens[6], tokens[7], stod(tokens[8]), 
                        stoull(tokens[9]), tokens[10]};
            } catch (...) {
                return {0, 0, "", "", "", 0, "", "", 0.0, 0, ""};
            }
        }
        return {0, 0, "", "", "", 0, "", "", 0.0, 0, ""};
    }
};

// 2. TRANSACTION MODEL
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

// 3. FIXED DEPOSIT MODEL
struct FixedDeposit {
    int fdId;
    int accountNumber;
    string depositType;
    double principal;
    int tenureMonths;
    double interestRate;
    double maturityAmount;
    string creationDate;

    string serialize() const {
        return to_string(fdId) + "|" + to_string(accountNumber) + "|" + depositType + "|" + 
               to_string(principal) + "|" + to_string(tenureMonths) + "|" + 
               to_string(interestRate) + "|" + to_string(maturityAmount) + "|" + creationDate;
    }

    static FixedDeposit deserialize(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() == 8) {
            try {
                return {stoi(tokens[0]), stoi(tokens[1]), tokens[2], stod(tokens[3]), 
                        stoi(tokens[4]), stod(tokens[5]), stod(tokens[6]), tokens[7]};
            } catch (...) {
                return {0, 0, "", 0.0, 0, 0.0, 0.0, ""};
            }
        }
        return {0, 0, "", 0.0, 0, 0.0, 0.0, ""};
    }
};

// 4. CUSTOMER MODEL
class Customer {
private:
    int customerId;
    string name;
    string mobile;
    string address;
    int age;
    string email;
    size_t customerPinHash;

public:
    Customer() : customerId(0), name(""), mobile(""), address(""), age(0), email(""), customerPinHash(0) {}

    Customer(int id, string n, string m, string a, int ag, string e, size_t pinH)
        : customerId(id), name(n), mobile(m), address(a), age(ag), email(e), customerPinHash(pinH) {}

    int getCustomerId() const { return customerId; }
    string getName() const { return name; }
    string getMobile() const { return mobile; }
    string getAddress() const { return address; }
    int getAge() const { return age; }
    string getEmail() const { return email; }

    bool verifyPin(const string& enteredPin) const {
        hash<string> hasher;
        return hasher(enteredPin) == customerPinHash;
    }

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
        return to_string(customerId) + "|" + name + "|" + mobile + "|" + address + "|" + 
               to_string(age) + "|" + email + "|" + to_string(customerPinHash);
    }

    static Customer deserialize(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        while (getline(ss, item, '|')) {
            tokens.push_back(item);
        }
        if (tokens.size() == 7) {
            try {
                return Customer(stoi(tokens[0]), tokens[1], tokens[2], tokens[3], 
                                stoi(tokens[4]), tokens[5], stoull(tokens[6]));
            } catch (...) {
                return Customer();
            }
        }
        return Customer();
    }
};

// 5. ACCOUNT BASE MODEL
class Account {
protected:
    int accountNumber;
    int customerId;
    double balance;
    string accountType;

public:
    Account() : accountNumber(0), customerId(0), balance(0.0), accountType("SAVINGS") {}

    Account(int accNum, int custId, double initialDeposit, string accType)
        : accountNumber(accNum), customerId(custId), balance(initialDeposit), accountType(accType) {}

    virtual ~Account() {}

    int getAccountNumber() const { return accountNumber; }
    int getCustomerId() const { return customerId; }
    double getBalance() const { return balance; }
    string getAccountType() const { return accountType; }

    void deposit(double amount) {
        balance += amount;
    }

    virtual bool canWithdraw(double amount) const = 0;
    virtual bool withdraw(double amount, vector<Transaction>& txLog) = 0;
    virtual void applyMonthlyCharges(vector<Transaction>& txLog) = 0;

    virtual string serialize() const {
        return to_string(accountNumber) + "|" + to_string(customerId) + "|" + to_string(balance) + "|" + accountType;
    }

    static Account* deserialize(const string& line);
};

// 6. SAVINGS ACCOUNT
class SavingsAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 500.0;
    static constexpr double LOW_BAL_FEE = 25.0;
    static constexpr double WITHDRAW_LIMIT = 20000.0;

    SavingsAccount(int accNum, int custId, double initialDeposit)
        : Account(accNum, custId, initialDeposit, "SAVINGS") {}

    bool canWithdraw(double amount) const override {
        if (amount > WITHDRAW_LIMIT || amount > balance) return false;
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
            cout << "[Notice] Minimum balance breached! Deducted fee of Rs " << LOW_BAL_FEE << ". Final: Rs " << balance << "\n";
        }
        return true;
    }

    void applyMonthlyCharges(vector<Transaction>& txLog) override {
        if (balance < MIN_BALANCE) {
            balance -= LOW_BAL_FEE;
            txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", LOW_BAL_FEE, balance});
            cout << ">> Acc " << accountNumber << " (Savings): Deducted Rs " << LOW_BAL_FEE << " for low balance.\n";
        }
    }
};

// 7. CURRENT ACCOUNT
class CurrentAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 10000.0;
    static constexpr double MONTHLY_PENALTY = 500.0;
    static constexpr double OVERDRAFT_LIMIT = 25000.0;

    CurrentAccount(int accNum, int custId, double initialDeposit)
        : Account(accNum, custId, initialDeposit, "CURRENT") {}

    bool canWithdraw(double amount) const override {
        if ((balance - amount) < -OVERDRAFT_LIMIT) return false;
        return true;
    }

    bool withdraw(double amount, vector<Transaction>& txLog) override {
        if ((balance - amount) < -OVERDRAFT_LIMIT) {
            cout << "[Error] Overdraft limit breached! Maximum credit is Rs -" 
                 << fixed << setprecision(2) << OVERDRAFT_LIMIT << "\n";
            return false;
        }

        balance -= amount;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "WITHDRAWAL", amount, balance});
        cout << "[Success] Withdrew Rs " << fixed << setprecision(2) << amount << ". Remaining: Rs " << balance << "\n";

        if (balance < MIN_BALANCE) {
            cout << "[Notice] Balance is below Rs 10000.00.\n";
        }
        return true;
    }

    void applyMonthlyCharges(vector<Transaction>& txLog) override {
        if (balance < MIN_BALANCE) {
            balance -= MONTHLY_PENALTY;
            txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", MONTHLY_PENALTY, balance});
            cout << ">> Acc " << accountNumber << " (Current): Below minimum. Deducted Rs " 
                 << MONTHLY_PENALTY << " penalty.\n";
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
    if (tokens.size() == 4) {
        try {
            int accNum = stoi(tokens[0]);
            int custId = stoi(tokens[1]);
            double bal = stod(tokens[2]);
            string type = tokens[3];

            if (type == "CURRENT") {
                return new CurrentAccount(accNum, custId, bal);
            } else {
                return new SavingsAccount(accNum, custId, bal);
            }
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// 8. BANK SYSTEM CONTROLLER
class BankSystem {
private:
    vector<Customer> customers;
    vector<Account*> accounts;
    vector<Transaction> transactions;
    vector<FixedDeposit> deposits;
    vector<AccountApplication> applications;

    const string customerFile = "customers.txt";
    const string accountFile = "accounts.txt";
    const string transactionFile = "transactions.txt";
    const string fdFile = "fds.txt";
    const string appFile = "applications.txt";
    const string adminPin = "admin123";

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

    vector<Account*> getAccountsForCustomer(int custId) {
        vector<Account*> linked;
        for (auto* a : accounts) {
            if (a->getCustomerId() == custId) linked.push_back(a);
        }
        return linked;
    }

    bool authenticateCustomer(Customer* cust) {
        string pin;
        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter Your Customer PIN (" << attempts << " attempts remaining): ";
            cin >> pin;
            if (cust->verifyPin(pin)) return true;
            attempts--;
            cout << "[Error] Incorrect PIN.\n";
        }
        cout << "[Access Denied] Authentication attempts exceeded.\n";
        return false;
    }

    void loadData() {
        customers.clear();
        for (auto* a : accounts) delete a;
        accounts.clear();
        transactions.clear();
        deposits.clear();
        applications.clear();

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

        ifstream fIn(fdFile);
        while (getline(fIn, line)) {
            if (!line.empty()) deposits.push_back(FixedDeposit::deserialize(line));
        }
        fIn.close();

        ifstream appIn(appFile);
        while (getline(appIn, line)) {
            if (!line.empty()) applications.push_back(AccountApplication::deserialize(line));
        }
        appIn.close();
    }

    void saveData() {
        ofstream cOut(customerFile);
        for (const auto& c : customers) cOut << c.serialize() << "\n";
        cOut.close();

        ofstream aOut(accountFile);
        for (const auto* a : accounts) aOut << a->serialize() << "\n";
        aOut.close();

        ofstream tOut(transactionFile);
        for (const auto& t : transactions) tOut << t.serialize() << "\n";
        tOut.close();

        ofstream fOut(fdFile);
        for (const auto& f : deposits) fOut << f.serialize() << "\n";
        fOut.close();

        ofstream appOut(appFile);
        for (const auto& ap : applications) appOut << ap.serialize() << "\n";
        appOut.close();
    }

public:
    BankSystem() {
        loadData();
    }

    ~BankSystem() {
        for (auto* a : accounts) delete a;
    }

    // ==========================================
    // ACCOUNT APPLICATION SUBMISSION
    // ==========================================
    void applyForAccount() {
        string name, mobile, address, email, pin;
        int age, typeChoice, existingCustId = 0;
        double initialDeposit;
        char hasExistingId;

        cout << "\n=========================================\n";
        cout << "       NEW ACCOUNT APPLICATION FORM      \n";
        cout << "=========================================\n";
        cout << "Do you already hold a Customer ID with us? (y/n): ";
        cin >> hasExistingId;
        clearBuffer();

        if (tolower(hasExistingId) == 'y') {
            cout << "Enter Existing Customer ID: ";
            if (cin >> existingCustId) {
                Customer* c = findCustomer(existingCustId);
                if (!c) {
                    cout << "[Error] Customer ID not found. Application aborted.\n";
                    clearBuffer();
                    return;
                }
                if (!authenticateCustomer(c)) return;
                name = c->getName();
                mobile = c->getMobile();
                address = c->getAddress();
                age = c->getAge();
                email = c->getEmail();
            } else {
                clearBuffer();
                return;
            }
        } else {
            cout << "Full Legal Name: ";
            getline(cin, name);

            cout << "Mobile: ";
            getline(cin, mobile);

            cout << "Address: ";
            getline(cin, address);

            cout << "Age: ";
            while (!(cin >> age) || age < 18) {
                cout << "Must be 18 or older: ";
                clearBuffer();
            }

            cout << "Email: ";
            clearBuffer();
            getline(cin, email);

            cout << "Set your 4-Digit Customer PIN: ";
            while (true) {
                cin >> pin;
                if (pin.length() == 4 && pin.find_first_not_of("0123456789") == string::npos) break;
                cout << "Invalid PIN. Must be exactly 4 digits: ";
                clearBuffer();
            }
        }

        cout << "Select Desired Account Type:\n";
        cout << "  1. Savings Account (Min Deposit: Rs 500)\n";
        cout << "  2. Current Account (Min Deposit: Rs 10,000)\n";
        cout << "Choice (1-2): ";
        while (!(cin >> typeChoice) || (typeChoice != 1 && typeChoice != 2)) {
            cout << "Select 1 or 2: ";
            clearBuffer();
        }

        string accType = (typeChoice == 1) ? "SAVINGS" : "CURRENT";
        double requiredMin = (typeChoice == 1) ? 500.0 : 10000.0;

        cout << "Planned Opening Deposit: Rs ";
        while (!(cin >> initialDeposit) || initialDeposit < requiredMin) {
            cout << "[Error] Minimum deposit is Rs " << fixed << setprecision(2) << requiredMin << ". Re-enter: Rs ";
            clearBuffer();
        }

        int appId = applications.empty() ? 3001 : applications.back().appId + 1;
        hash<string> hasher;
        size_t pHash = (hasExistingId == 'y') ? 0 : hasher(pin);

        applications.push_back({appId, existingCustId, name, mobile, address, age, email, accType, initialDeposit, pHash, "PENDING"});
        saveData();

        cout << "\n[Application Submitted Successfully!]\n";
        cout << "Your Application ID: " << appId << "\n";
        cout << "Status: PENDING REVIEW by bank administration.\n";
    }

    // ==========================================
    // CUSTOMER PORTAL (RELATIONAL 1:N)
    // ==========================================
    void customerPortal() {
        int custId;
        cout << "\n--- Customer Secure Gateway ---\n";
        cout << "Enter Your Customer ID: ";
        if (!(cin >> custId)) { clearBuffer(); return; }

        Customer* cust = findCustomer(custId);
        if (!cust) {
            cout << "[Error] Customer ID not found.\n";
            return;
        }

        if (!authenticateCustomer(cust)) return;

        int choice = 0;
        while (choice != 3) {
            vector<Account*> myAccounts = getAccountsForCustomer(custId);

            cout << "\n=========================================\n";
            cout << "       CUSTOMER PORTAL: " << cust->getName() << "\n";
            cout << "       Customer ID: " << custId << "\n";
            cout << "=========================================\n";
            cout << "1. Manage Accounts (" << myAccounts.size() << " Active)\n";
            cout << "2. Internal Transfer (Between My Own Accounts)\n";
            cout << "3. Logout\n";
            cout << "Select (1-3): ";

            if (!(cin >> choice)) { clearBuffer(); continue; }

            if (choice == 1) {
                if (myAccounts.empty()) {
                    cout << "[Notice] No active accounts found. Apply for one from the main menu.\n";
                    continue;
                }

                cout << "\nLinked Accounts:\n";
                for (size_t i = 0; i < myAccounts.size(); ++i) {
                    cout << "  " << (i + 1) << ". Account #" << myAccounts[i]->getAccountNumber() 
                         << " (" << myAccounts[i]->getAccountType() << ") | Balance: Rs " 
                         << fixed << setprecision(2) << myAccounts[i]->getBalance() << "\n";
                }
                cout << "Select an Account (1-" << myAccounts.size() << ") or 0 to return: ";
                int accIdx;
                if (cin >> accIdx && accIdx > 0 && accIdx <= static_cast<int>(myAccounts.size())) {
                    manageSingleAccount(myAccounts[accIdx - 1], cust);
                }
            } else if (choice == 2) {
                executeInternalTransfer(myAccounts);
            }
        }
    }

    void manageSingleAccount(Account* acc, Customer* cust) {
        int choice = 0;
        while (choice != 8) {
            cout << "\n-----------------------------------------\n";
            cout << "  DASHBOARD: Account #" << acc->getAccountNumber() 
                 << " (" << acc->getAccountType() << ")\n";
            cout << "  Current Balance: Rs " << fixed << setprecision(2) << acc->getBalance() << "\n";
            cout << "-----------------------------------------\n";
            cout << "1. View Statement\n";
            cout << "2. Deposit Funds\n";
            cout << "3. Withdraw Funds\n";
            cout << "4. Third-Party Transfer\n";
            cout << "5. Open FD / RD Term Deposit\n";
            cout << "6. View Term Deposits\n";
            cout << "7. Export Statement to CSV (Excel)\n";
            cout << "8. Return to Account List\n";
            cout << "Select (1-8): ";

            if (!(cin >> choice)) { clearBuffer(); continue; }

            switch (choice) {
                case 1: displayAccountDetails(acc, cust); break;
                case 2: deposit(acc); break;
                case 3: withdraw(acc); break;
                case 4: transferFunds(acc); break;
                case 5: createDeposit(acc); break;
                case 6: viewCustomerDeposits(acc->getAccountNumber()); break;
                case 7: exportStatementCSV(acc, cust); break;
                case 8: break;
                default: cout << "Invalid option.\n"; break;
            }
        }
    }

    void executeInternalTransfer(const vector<Account*>& myAccounts) {
        if (myAccounts.size() < 2) {
            cout << "[Error] You need at least 2 active accounts for internal self-transfers.\n";
            return;
        }

        cout << "\n--- Internal Transfer Between Your Accounts ---\n";
        for (size_t i = 0; i < myAccounts.size(); ++i) {
            cout << "  " << (i + 1) << ". #" << myAccounts[i]->getAccountNumber() 
                 << " (" << myAccounts[i]->getAccountType() << ") - Rs " << myAccounts[i]->getBalance() << "\n";
        }

        int srcIdx, destIdx;
        cout << "Select Source Account (1-" << myAccounts.size() << "): ";
        cin >> srcIdx;
        cout << "Select Destination Account (1-" << myAccounts.size() << "): ";
        cin >> destIdx;

        if (srcIdx < 1 || srcIdx > static_cast<int>(myAccounts.size()) || 
            destIdx < 1 || destIdx > static_cast<int>(myAccounts.size()) || srcIdx == destIdx) {
            cout << "[Error] Invalid selection.\n";
            return;
        }

        Account* src = myAccounts[srcIdx - 1];
        Account* dest = myAccounts[destIdx - 1];
        double amount;

        cout << "Enter Transfer Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        if (!src->canWithdraw(amount)) {
            cout << "[Transfer Failed] Insufficient balance or overdraft limit reached on source account.\n";
            return;
        }

        string timeNow = getCurrentTimestamp();
        src->withdraw(amount, transactions);
        transactions.back().type = "INTERNAL_TO_" + to_string(dest->getAccountNumber());

        dest->deposit(amount);
        transactions.push_back({dest->getAccountNumber(), timeNow, "INTERNAL_FROM_" + to_string(src->getAccountNumber()), amount, dest->getBalance()});

        saveData();
        cout << "[Success] Transferred Rs " << fixed << setprecision(2) << amount << " between your accounts.\n";
    }

    void deposit(Account* acc) {
        double amount;
        cout << "Enter Deposit Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        acc->deposit(amount);
        transactions.push_back({acc->getAccountNumber(), getCurrentTimestamp(), "DEPOSIT", amount, acc->getBalance()});
        saveData();
        cout << "[Success] Deposited Rs " << fixed << setprecision(2) << amount 
             << ". New Balance: Rs " << acc->getBalance() << "\n";
    }

    void withdraw(Account* acc) {
        double amount;
        cout << "Enter Withdrawal Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        if (acc->withdraw(amount, transactions)) {
            saveData();
        }
    }

    void transferFunds(Account* sender) {
        int receiverAccNum;
        double amount;

        cout << "\nEnter Beneficiary Account Number: ";
        if (!(cin >> receiverAccNum)) { clearBuffer(); return; }

        if (sender->getAccountNumber() == receiverAccNum) {
            cout << "[Error] Source and destination accounts cannot be identical.\n";
            return;
        }

        Account* receiver = findAccount(receiverAccNum);
        if (!receiver) {
            cout << "[Error] Beneficiary account does not exist.\n";
            return;
        }

        cout << "Enter Transfer Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid amount.\n";
            clearBuffer();
            return;
        }

        if (!sender->canWithdraw(amount)) {
            cout << "[Transfer Failed] Insufficient balance or limit clearance.\n";
            return;
        }

        string timeNow = getCurrentTimestamp();
        sender->withdraw(amount, transactions);
        transactions.back().type = "TRANSFER_TO_" + to_string(receiverAccNum);

        receiver->deposit(amount);
        transactions.push_back({receiverAccNum, timeNow, "TRANSFER_FROM_" + to_string(sender->getAccountNumber()), amount, receiver->getBalance()});

        saveData();
        cout << "[Success] Rs " << fixed << setprecision(2) << amount 
             << " transferred to Account " << receiverAccNum << ".\n";
    }

    void createDeposit(Account* acc) {
        int dTypeChoice, tenure;
        double principal;

        cout << "\n--- Fixed / Recurring Term Deposit Engine ---\n";
        cout << "1. Fixed Deposit (FD)\n";
        cout << "2. Recurring Deposit (RD)\n";
        cout << "Select (1-2): ";
        if (!(cin >> dTypeChoice) || (dTypeChoice != 1 && dTypeChoice != 2)) {
            cout << "[Error] Invalid selection.\n";
            clearBuffer();
            return;
        }

        string depType = (dTypeChoice == 1) ? "FD" : "RD";

        cout << "Enter Investment Principal: Rs ";
        if (!(cin >> principal) || principal <= 0) {
            cout << "[Error] Invalid principal.\n";
            clearBuffer();
            return;
        }

        if (principal > acc->getBalance()) {
            cout << "[Error] Account balance insufficient to fund investment.\n";
            return;
        }

        cout << "Enter Tenure in Months (e.g., 6, 12, 24, 36): ";
        if (!(cin >> tenure) || tenure <= 0) {
            cout << "[Error] Invalid tenure.\n";
            clearBuffer();
            return;
        }

        double rate = 5.0;
        if (tenure >= 12 && tenure < 24) rate = 6.5;
        else if (tenure >= 24 && tenure < 36) rate = 7.1;
        else if (tenure >= 36) rate = 7.5;

        double timeInYears = static_cast<double>(tenure) / 12.0;
        double maturity = principal * pow(1.0 + (rate / (100.0 * 4.0)), 4.0 * timeInYears);

        acc->withdraw(principal, transactions);
        transactions.back().type = depType + "_CREATION";

        int fdId = deposits.empty() ? 9001 : deposits.back().fdId + 1;
        deposits.push_back({fdId, acc->getAccountNumber(), depType, principal, tenure, rate, maturity, getCurrentTimestamp()});
        saveData();

        cout << "\n[" << depType << " Successfully Opened]\n";
        cout << "Deposit ID     : " << fdId << "\n";
        cout << "Interest Rate  : " << rate << "% p.a.\n";
        cout << "Maturity Yield : Rs " << fixed << setprecision(2) << maturity << "\n";
    }

    void viewCustomerDeposits(int accNum) {
        cout << "\n--- Active FD / RD Holdings (Account #" << accNum << ") ---\n";
        bool hasDeposits = false;
        cout << left << setw(8) << "ID" 
             << setw(6) << "Type" 
             << setw(14) << "Principal" 
             << setw(10) << "Tenure" 
             << setw(8) << "Rate" 
             << setw(16) << "Maturity (Rs)" 
             << setw(21) << "Created Date" << "\n";
        cout << string(83, '-') << "\n";

        for (const auto& d : deposits) {
            if (d.accountNumber == accNum) {
                hasDeposits = true;
                cout << left << setw(8) << d.fdId
                     << setw(6) << d.depositType
                     << "Rs " << setw(11) << fixed << setprecision(2) << d.principal
                     << setw(10) << (to_string(d.tenureMonths) + " mos")
                     << setw(8) << (to_string(d.interestRate).substr(0, 4) + "%")
                     << "Rs " << setw(13) << fixed << setprecision(2) << d.maturityAmount
                     << setw(21) << d.creationDate << "\n";
            }
        }
        if (!hasDeposits) cout << "No term deposits linked to this account.\n";
    }

    void exportStatementCSV(Account* acc, Customer* cust) {
        string filename = "statement_" + to_string(acc->getAccountNumber()) + ".csv";
        ofstream csvOut(filename);

        if (!csvOut.is_open()) {
            cout << "[Error] Unable to generate CSV file.\n";
            return;
        }

        csvOut << "OFFICIAL BANK STATEMENT\n";
        csvOut << "Generated On," << getCurrentTimestamp() << "\n\n";

        if (cust) {
            csvOut << "CUSTOMER PROFILE\n";
            csvOut << "Customer ID," << cust->getCustomerId() << "\n";
            csvOut << "Name," << cust->getName() << "\n";
            csvOut << "Mobile," << cust->getMobile() << "\n";
            csvOut << "Email," << cust->getEmail() << "\n\n";
        }

        csvOut << "ACCOUNT DETAILS\n";
        csvOut << "Account Number," << acc->getAccountNumber() << "\n";
        csvOut << "Account Type," << acc->getAccountType() << "\n";
        csvOut << "Current Balance,Rs " << fixed << setprecision(2) << acc->getBalance() << "\n\n";

        csvOut << "TRANSACTION LEDGER\n";
        csvOut << "Date & Time,Transaction Type,Amount (Rs),Balance After (Rs)\n";

        for (const auto& tx : transactions) {
            if (tx.accountNumber == acc->getAccountNumber()) {
                csvOut << tx.timestamp << ","
                       << tx.type << ","
                       << fixed << setprecision(2) << tx.amount << ","
                       << fixed << setprecision(2) << tx.balanceAfter << "\n";
            }
        }

        csvOut.close();
        cout << "[Success] Statement exported to " << filename << " (Ready for Excel/Sheets)\n";
    }

    void displayAccountDetails(Account* acc, Customer* cust) {
        cout << "\n======================================================================\n";
        cout << "                       ACCOUNT SUMMARY STATEMENT                      \n";
        cout << "======================================================================\n";
        cout << "Account No      : " << acc->getAccountNumber() << "\n";
        cout << "Account Type    : " << acc->getAccountType() << "\n";
        cout << "Current Balance : Rs " << fixed << setprecision(2) << acc->getBalance() << "\n";
        if (cust) cust->displayProfile();

        cout << "\n--- Transaction History ---\n";
        bool hasTx = false;
        cout << left << setw(21) << "Date & Time"
             << setw(22) << "Type" 
             << setw(14) << "Amount (Rs)" 
             << setw(16) << "Balance After" << "\n";
        cout << string(73, '-') << "\n";

        for (const auto& tx : transactions) {
            if (tx.accountNumber == acc->getAccountNumber()) {
                hasTx = true;
                cout << left << setw(21) << tx.timestamp
                     << setw(22) << tx.type 
                     << setw(14) << fixed << setprecision(2) << tx.amount 
                     << setw(16) << fixed << setprecision(2) << tx.balanceAfter << "\n";
            }
        }
        if (!hasTx) cout << "No transactions recorded yet.\n";
        cout << "======================================================================\n";
    }

    // ==========================================
    // ADMIN / MANAGER PORTAL
    // ==========================================
    void adminPortal() {
        string pin;
        cout << "\n--- Manager Security Authentication ---\n";
        cout << "Enter Bank Admin Key: ";
        cin >> pin;

        if (pin != adminPin) {
            cout << "[Security Alert] Unauthorized access rejected.\n";
            return;
        }

        int choice = 0;
        while (choice != 8) {
            cout << "\n=========================================\n";
            cout << "       ADMIN & AUDIT CONTROL PANEL       \n";
            cout << "=========================================\n";
            cout << "1. Review Pending Account Applications (" << countPendingApplications() << " Pending)\n";
            cout << "2. View Bank Capital Reserves & Audits\n";
            cout << "3. List All Customers & Linked Accounts\n";
            cout << "4. Update Customer Information\n";
            cout << "5. Run Monthly Account Maintenance Audit\n";
            cout << "6. Inspect Master Statement of an Account\n";
            cout << "7. Direct Admin Account Onboarding\n";
            cout << "8. Return to Gateway\n";
            cout << "Select (1-8): ";

            if (!(cin >> choice)) { clearBuffer(); continue; }

            switch (choice) {
                case 1: processApplications(); break;
                case 2: viewBankReserves(); break;
                case 3: listAllAccounts(); break;
                case 4: updateCustomer(); break;
                case 5: runMonthlyMaintenanceCycle(); break;
                case 6: inspectMasterStatement(); break;
                case 7: directAdminOnboarding(); break;
                case 8: cout << "Exited admin control panel.\n"; break;
                default: cout << "Invalid option.\n"; break;
            }
        }
    }

    int countPendingApplications() {
        int c = 0;
        for (const auto& a : applications) if (a.status == "PENDING") c++;
        return c;
    }

    void processApplications() {
        cout << "\n--- Review Customer Account Applications ---\n";
        bool hasPending = false;
        for (auto& app : applications) {
            if (app.status == "PENDING") {
                hasPending = true;
                cout << "\n[Application #" << app.appId << "]\n";
                cout << "Customer Name : " << app.name << "\n";
                cout << "Age           : " << app.age << " | Mobile: " << app.mobile << "\n";
                cout << "Email         : " << app.email << "\n";
                cout << "Address       : " << app.address << "\n";
                cout << "Account Type  : " << app.accountType << "\n";
                cout << "Opening Dep   : Rs " << fixed << setprecision(2) << app.initialDeposit << "\n";
                if (app.customerId != 0) {
                    cout << "Linking to    : Existing Customer ID " << app.customerId << "\n";
                } else {
                    cout << "New Profile   : New Customer Registration\n";
                }

                char decision;
                cout << "Action: [A]pprove, [R]eject, [S]kip: ";
                cin >> decision;
                decision = toupper(decision);

                if (decision == 'A') {
                    app.status = "APPROVED";
                    int assignedCustId = app.customerId;
                    if (assignedCustId == 0) {
                        assignedCustId = customers.empty() ? 5001 : customers.back().getCustomerId() + 1;
                        customers.emplace_back(assignedCustId, app.name, app.mobile, app.address, app.age, app.email, app.pinHash);
                    }

                    int newAccNum = accounts.empty() ? 10001 : accounts.back()->getAccountNumber() + 1;
                    if (app.accountType == "SAVINGS") {
                        accounts.push_back(new SavingsAccount(newAccNum, assignedCustId, app.initialDeposit));
                    } else {
                        accounts.push_back(new CurrentAccount(newAccNum, assignedCustId, app.initialDeposit));
                    }

                    transactions.push_back({newAccNum, getCurrentTimestamp(), "INITIAL_DEP", app.initialDeposit, app.initialDeposit});
                    saveData();

                    cout << "[Approved] Account #" << newAccNum << " assigned to Customer ID " << assignedCustId << "\n";
                } else if (decision == 'R') {
                    app.status = "REJECTED";
                    saveData();
                    cout << "[Rejected] Application #" << app.appId << " was marked rejected.\n";
                }
            }
        }
        if (!hasPending) cout << "No pending applications to review.\n";
    }

    void directAdminOnboarding() {
        string name, mobile, address, email, pin;
        int age, typeChoice;
        double initialDeposit;

        cout << "\n--- Direct In-Branch Onboarding ---\n";
        cout << "Full Name: ";
        clearBuffer();
        getline(cin, name);

        cout << "Mobile: ";
        getline(cin, mobile);

        cout << "Address: ";
        getline(cin, address);

        cout << "Age: ";
        while (!(cin >> age) || age < 18) {
            cout << "Must be 18 or older: ";
            clearBuffer();
        }

        cout << "Email: ";
        clearBuffer();
        getline(cin, email);

        cout << "Set Customer 4-Digit PIN: ";
        while (true) {
            cin >> pin;
            if (pin.length() == 4 && pin.find_first_not_of("0123456789") == string::npos) break;
            cout << "Invalid PIN. Must be 4 digits: ";
            clearBuffer();
        }

        cout << "Select Account Type (1. Savings | 2. Current): ";
        while (!(cin >> typeChoice) || (typeChoice != 1 && typeChoice != 2)) {
            cout << "Select 1 or 2: ";
            clearBuffer();
        }

        double requiredDeposit = (typeChoice == 1) ? 500.0 : 10000.0;
        cout << "Initial Deposit: Rs ";
        while (!(cin >> initialDeposit) || initialDeposit < requiredDeposit) {
            cout << "[Error] Minimum deposit is Rs " << fixed << setprecision(2) << requiredDeposit << ": Rs ";
            clearBuffer();
        }

        int custId = customers.empty() ? 5001 : customers.back().getCustomerId() + 1;
        int accNum = accounts.empty() ? 10001 : accounts.back()->getAccountNumber() + 1;

        hash<string> hasher;
        customers.emplace_back(custId, name, mobile, address, age, email, hasher(pin));

        if (typeChoice == 1) {
            accounts.push_back(new SavingsAccount(accNum, custId, initialDeposit));
        } else {
            accounts.push_back(new CurrentAccount(accNum, custId, initialDeposit));
        }

        transactions.push_back({accNum, getCurrentTimestamp(), "INITIAL_DEP", initialDeposit, initialDeposit});
        saveData();

        cout << "\n[Account Created Immediately]\n";
        cout << "Customer ID : " << custId << "\n";
        cout << "Account No  : " << accNum << "\n";
    }

    void viewBankReserves() {
        double totalLiquid = 0.0, totalLockedFD = 0.0;
        for (const auto* a : accounts) totalLiquid += a->getBalance();
        for (const auto& d : deposits) totalLockedFD += d.principal;

        cout << "\n=========================================\n";
        cout << "         CENTRAL CAPITAL RESERVES        \n";
        cout << "=========================================\n";
        cout << "Active Bank Accounts : " << accounts.size() << "\n";
        cout << "Registered Customers : " << customers.size() << "\n";
        cout << "Liquid Account Funds : Rs " << fixed << setprecision(2) << totalLiquid << "\n";
        cout << "Locked Term Deposits : Rs " << fixed << setprecision(2) << totalLockedFD << "\n";
        cout << "Total Bank Assets    : Rs " << fixed << setprecision(2) << (totalLiquid + totalLockedFD) << "\n";
        cout << "=========================================\n";
    }

    void listAllAccounts() {
        cout << "\n--- Master Customer & Linked Accounts Roster ---\n";
        cout << left << setw(10) << "Cust ID" 
             << setw(20) << "Holder Name" 
             << setw(12) << "Acc No" 
             << setw(12) << "Type" 
             << setw(16) << "Balance (Rs)" << "\n";
        cout << string(70, '-') << "\n";

        for (const auto& c : customers) {
            vector<Account*> linked = getAccountsForCustomer(c.getCustomerId());
            if (linked.empty()) {
                cout << left << setw(10) << c.getCustomerId()
                     << setw(20) << c.getName()
                     << setw(12) << "NO ACCOUNTS"
                     << setw(12) << "-"
                     << setw(16) << "-" << "\n";
            } else {
                for (const auto* a : linked) {
                    cout << left << setw(10) << c.getCustomerId()
                         << setw(20) << c.getName()
                         << setw(12) << a->getAccountNumber()
                         << setw(12) << a->getAccountType()
                         << "Rs " << setw(13) << fixed << setprecision(2) << a->getBalance() << "\n";
                }
            }
        }
    }

    void updateCustomer() {
        int custId;
        cout << "\nEnter Customer ID to Modify: ";
        if (!(cin >> custId)) { clearBuffer(); return; }

        Customer* cust = findCustomer(custId);
        if (!cust) { cout << "[Error] Customer ID not found.\n"; return; }

        int choice = 0;
        do {
            cout << "\n--- Update Profile (" << cust->getName() << ") ---\n";
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

    void runMonthlyMaintenanceCycle() {
        cout << "\n[Executing System-Wide Maintenance Audit...]\n";
        for (auto* acc : accounts) {
            acc->applyMonthlyCharges(transactions);
        }
        saveData();
        cout << "[Complete] System audit processed and written to disk.\n";
    }

    void inspectMasterStatement() {
        int accNum;
        cout << "\nEnter Account Number to Inspect: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        Customer* c = findCustomer(acc->getCustomerId());
        displayAccountDetails(acc, c);
    }
};

int main() {
    BankSystem system;
    int choice = 0;

    while (choice != 4) {
        cout << "\n=========================================\n";
        cout << "       CENTRAL SECURE BANKING GATEWAY    \n";
        cout << "=========================================\n";
        cout << "1. Customer Portal (Login with Customer ID)\n";
        cout << "2. Apply for New Account / Registration\n";
        cout << "3. Admin / Manager Portal (Approvals & Audits)\n";
        cout << "4. Exit System\n";
        cout << "Select Gateway (1-4): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.customerPortal(); break;
            case 2: system.applyForAccount(); break;
            case 3: system.adminPortal(); break;
            case 4: cout << "Securing database and shutting down. Goodbye!\n"; break;
            default: cout << "Invalid selection.\n"; break;
        }
    }
    return 0;
}
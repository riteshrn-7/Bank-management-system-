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

// 2. FIXED DEPOSIT MODEL
struct FixedDeposit {
    int fdId;
    int accountNumber;
    string depositType; // "FD" or "RD"
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

// 3. CUSTOMER MODEL
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

// 4. ACCOUNT BASE MODEL
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

// 5. SAVINGS ACCOUNT
class SavingsAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 500.0;
    static constexpr double LOW_BAL_FEE = 25.0;
    static constexpr double WITHDRAW_LIMIT = 20000.0;

    SavingsAccount(int accNum, int custId, double initialDeposit, size_t hashVal)
        : Account(accNum, custId, initialDeposit, "SAVINGS", hashVal) {}

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

// 6. CURRENT ACCOUNT
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

// 7. BANK SYSTEM CONTROLLER
class BankSystem {
private:
    vector<Customer> customers;
    vector<Account*> accounts;
    vector<Transaction> transactions;
    vector<FixedDeposit> deposits;

    const string customerFile = "customers.txt";
    const string accountFile = "accounts.txt";
    const string transactionFile = "transactions.txt";
    const string fdFile = "fds.txt";
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
        deposits.clear();

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
    }

public:
    BankSystem() {
        loadData();
    }

    ~BankSystem() {
        for (auto* a : accounts) delete a;
    }

    // ==========================================
    // CUSTOMER PORTAL METHODS
    // ==========================================

    void customerPortal() {
        int accNum;
        cout << "\n--- Customer Portal Login ---\n";
        cout << "Enter Your Account Number: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        if (!authenticate(acc)) return;

        int choice = 0;
        while (choice != 8) {
            cout << "\n=========================================\n";
            cout << "       CUSTOMER ACCOUNT DASHBOARD       \n";
            cout << "  Account: " << acc->getAccountNumber() << " (" << acc->getAccountType() << ")\n";
            cout << "=========================================\n";
            cout << "1. View Statement & Profile\n";
            cout << "2. Quick Balance\n";
            cout << "3. Deposit Funds\n";
            cout << "4. Withdraw Funds\n";
            cout << "5. Fund Transfer\n";
            cout << "6. Open Fixed Deposit / RD\n";
            cout << "7. View Active Deposits\n";
            cout << "8. Logout\n";
            cout << "Select (1-8): ";

            if (!(cin >> choice)) {
                clearBuffer();
                continue;
            }

            switch (choice) {
                case 1: displayAccountDetails(acc); break;
                case 2:
                    cout << "Current Balance: Rs " << fixed << setprecision(2) << acc->getBalance() << "\n";
                    break;
                case 3: deposit(acc); break;
                case 4: withdraw(acc); break;
                case 5: transferFunds(acc); break;
                case 6: createDeposit(acc); break;
                case 7: viewCustomerDeposits(acc->getAccountNumber()); break;
                case 8: cout << "Logged out from customer portal.\n"; break;
                default: cout << "Invalid choice.\n"; break;
            }
        }
    }

    void deposit(Account* acc) {
        double amount;
        cout << "Enter Deposit Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid deposit amount.\n";
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
            cout << "[Error] Invalid withdrawal amount.\n";
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
            cout << "[Error] Source and Destination accounts cannot be identical.\n";
            return;
        }

        Account* receiver = findAccount(receiverAccNum);
        if (!receiver) {
            cout << "[Error] Destination account does not exist.\n";
            return;
        }

        cout << "Enter Transfer Amount: Rs ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "[Error] Invalid transfer amount.\n";
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

        cout << "\n--- Investment Products Engine ---\n";
        cout << "1. Fixed Deposit (FD) - Lump sum compound deposit\n";
        cout << "2. Recurring Deposit (RD) - Monthly investment model\n";
        cout << "Select (1-2): ";
        if (!(cin >> dTypeChoice) || (dTypeChoice != 1 && dTypeChoice != 2)) {
            cout << "[Error] Invalid selection.\n";
            clearBuffer();
            return;
        }

        string depType = (dTypeChoice == 1) ? "FD" : "RD";

        cout << "Enter Principal / Investment Amount: Rs ";
        if (!(cin >> principal) || principal <= 0) {
            cout << "[Error] Invalid principal amount.\n";
            clearBuffer();
            return;
        }

        if (principal > acc->getBalance()) {
            cout << "[Error] Insufficient balance in linked account to fund investment.\n";
            return;
        }

        cout << "Enter Tenure in Months (e.g., 6, 12, 24, 36): ";
        if (!(cin >> tenure) || tenure <= 0) {
            cout << "[Error] Invalid tenure.\n";
            clearBuffer();
            return;
        }

        // Slab-based Interest Matrix
        double rate = 5.0;
        if (tenure >= 12 && tenure < 24) rate = 6.5;
        else if (tenure >= 24 && tenure < 36) rate = 7.1;
        else if (tenure >= 36) rate = 7.5;

        // Compound Maturity: A = P(1 + r/n)^(n*t), compounded quarterly (n=4)
        double timeInYears = static_cast<double>(tenure) / 12.0;
        double maturity = principal * pow(1.0 + (rate / (100.0 * 4.0)), 4.0 * timeInYears);

        // Deduct principal from source account
        acc->withdraw(principal, transactions);
        transactions.back().type = depType + "_CREATION";

        int fdId = deposits.empty() ? 9001 : deposits.back().fdId + 1;
        deposits.push_back({fdId, acc->getAccountNumber(), depType, principal, tenure, rate, maturity, getCurrentTimestamp()});
        saveData();

        cout << "\n[" << depType << " Successfully Created]\n";
        cout << "Deposit ID     : " << fdId << "\n";
        cout << "Interest Rate  : " << rate << "% p.a.\n";
        cout << "Maturity Value : Rs " << fixed << setprecision(2) << maturity << "\n";
    }

    void viewCustomerDeposits(int accNum) {
        cout << "\n--- Active FD / RD Holdings for Account " << accNum << " ---\n";
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
        if (!hasDeposits) cout << "No FD/RD records linked to this account.\n";
    }

    void displayAccountDetails(Account* acc) {
        Customer* cust = findCustomer(acc->getCustomerId());
        if (!cust) { cout << "[Error] Customer profile missing.\n"; return; }

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
            if (tx.accountNumber == acc->getAccountNumber()) {
                hasTx = true;
                cout << left << setw(21) << tx.timestamp
                     << setw(20) << tx.type 
                     << setw(14) << fixed << setprecision(2) << tx.amount 
                     << setw(16) << fixed << setprecision(2) << tx.balanceAfter << "\n";
            }
        }
        if (!hasTx) cout << "No transactions recorded yet.\n";
        cout << "======================================================================\n";
    }

    // ==========================================
    // ADMIN / MANAGER PORTAL METHODS
    // ==========================================

    void adminPortal() {
        string pin;
        cout << "\n--- Manager Security Authentication ---\n";
        cout << "Enter Bank Admin Key: ";
        cin >> pin;

        if (pin != adminPin) {
            cout << "[Security Alert] Unauthorized access attempt blocked.\n";
            return;
        }

        int choice = 0;
        while (choice != 7) {
            cout << "\n=========================================\n";
            cout << "       ADMIN & AUDIT CONTROL PANEL       \n";
            cout << "=========================================\n";
            cout << "1. Open New Customer Account\n";
            cout << "2. View Bank Capital Reserves & Audits\n";
            cout << "3. List All Customers & Linked Accounts\n";
            cout << "4. Update Customer Information\n";
            cout << "5. Run Monthly Account Maintenance Audit\n";
            cout << "6. Inspect Master Ledger / System Statements\n";
            cout << "7. Return to Main Screen\n";
            cout << "Select (1-7): ";

            if (!(cin >> choice)) {
                clearBuffer();
                continue;
            }

            switch (choice) {
                case 1: onboardCustomer(); break;
                case 2: viewBankReserves(); break;
                case 3: listAllAccounts(); break;
                case 4: updateCustomer(); break;
                case 5: runMonthlyMaintenanceCycle(); break;
                case 6: inspectMasterStatement(); break;
                case 7: cout << "Exited admin control panel.\n"; break;
                default: cout << "Invalid option.\n"; break;
            }
        }
    }

    void onboardCustomer() {
        string name, mobile, address, email, pin;
        int age, accountTypeChoice;
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

        cout << "Assign Customer a 4-Digit PIN: ";
        while (true) {
            cin >> pin;
            if (pin.length() == 4 && pin.find_first_not_of("0123456789") == string::npos) break;
            cout << "Invalid PIN. Must be 4 digits: ";
            clearBuffer();
        }

        cout << "Select Account Type:\n";
        cout << "  1. Savings Account (Minimum Deposit: Rs 500)\n";
        cout << "  2. Current Account (Minimum Deposit: Rs 10,000 | Low-Balance Monthly Fee: Rs 500)\n";
        cout << "Choice (1-2): ";
        while (!(cin >> accountTypeChoice) || (accountTypeChoice != 1 && accountTypeChoice != 2)) {
            cout << "Invalid choice. Select 1 or 2: ";
            clearBuffer();
        }

        double requiredDeposit = (accountTypeChoice == 1) ? 500.0 : 10000.0;
        cout << "Initial Opening Deposit: Rs ";
        while (!(cin >> initialDeposit) || initialDeposit < requiredDeposit) {
            cout << "[Error] Minimum deposit is Rs " << fixed << setprecision(2) << requiredDeposit << ". Re-enter: Rs ";
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

    void viewBankReserves() {
        double totalLiquid = 0.0;
        double totalLockedFD = 0.0;

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
        cout << "\n--- Master Account Roster ---\n";
        cout << left << setw(12) << "Acc No" 
             << setw(10) << "Cust ID" 
             << setw(20) << "Holder Name" 
             << setw(12) << "Type" 
             << setw(16) << "Balance (Rs)" << "\n";
        cout << string(70, '-') << "\n";

        for (const auto* a : accounts) {
            Customer* c = findCustomer(a->getCustomerId());
            string name = c ? c->getName() : "Unknown";
            cout << left << setw(12) << a->getAccountNumber()
                 << setw(10) << a->getCustomerId()
                 << setw(20) << name
                 << setw(12) << a->getAccountType()
                 << "Rs " << setw(13) << fixed << setprecision(2) << a->getBalance() << "\n";
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
            cout << "\n--- Update Records (" << cust->getName() << ") ---\n";
            cout << "1. Mobile | 2. Address | 3. Email | 4. Save & Exit\n";
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
        cout << "[Complete] System audits executed and penalty debits committed to disk.\n";
    }

    void inspectMasterStatement() {
        int accNum;
        cout << "\nEnter Account Number to Inspect: ";
        if (!(cin >> accNum)) { clearBuffer(); return; }

        Account* acc = findAccount(accNum);
        if (!acc) { cout << "[Error] Account not found.\n"; return; }

        displayAccountDetails(acc);
    }
};

int main() {
    BankSystem system;
    int roleChoice = 0;

    while (roleChoice != 3) {
        cout << "\n=========================================\n";
        cout << "       CENTRAL SECURE BANKING GATEWAY    \n";
        cout << "=========================================\n";
        cout << "1. Customer Portal (Personal Banking)\n";
        cout << "2. Admin / Manager Portal (Bank Audits)\n";
        cout << "3. Exit System\n";
        cout << "Select Portal (1-3): ";

        if (!(cin >> roleChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (roleChoice) {
            case 1: system.customerPortal(); break;
            case 2: system.adminPortal(); break;
            case 3: cout << "Securing database and shutting down. Goodbye!\n"; break;
            default: cout << "Invalid portal choice.\n"; break;
        }
    }
    return 0;
}
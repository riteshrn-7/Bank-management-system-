#include "Account.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>

Account::Account() : accountNumber(0), customerId(0), balance(0.0), accountType("SAVINGS") {}

Account::Account(int accNum, int custId, double initialDeposit, std::string accType)
    : accountNumber(accNum), customerId(custId), balance(initialDeposit), accountType(accType) {}

int Account::getAccountNumber() const { return accountNumber; }
int Account::getCustomerId() const { return customerId; }
double Account::getBalance() const { return balance; }
std::string Account::getAccountType() const { return accountType; }

void Account::deposit(double amount) { balance += amount; }

std::string Account::serialize() const {
    return std::to_string(accountNumber) + "|" + std::to_string(customerId) + "|" + std::to_string(balance) + "|" + accountType;
}

Account* Account::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);
    if (tokens.size() == 4) {
        try {
            int accNum = std::stoi(tokens[0]);
            int custId = std::stoi(tokens[1]);
            double bal = std::stod(tokens[2]);
            std::string type = tokens[3];

            if (type == "CURRENT") return new CurrentAccount(accNum, custId, bal);
            return new SavingsAccount(accNum, custId, bal);
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// Savings Account
SavingsAccount::SavingsAccount(int accNum, int custId, double initialDeposit)
    : Account(accNum, custId, initialDeposit, "SAVINGS") {}

bool SavingsAccount::canWithdraw(double amount) const {
    return !(amount > WITHDRAW_LIMIT || amount > balance);
}

bool SavingsAccount::withdraw(double amount, std::vector<Transaction>& txLog) {
    if (amount > WITHDRAW_LIMIT) {
        std::cout << "[Error] Exceeds single withdrawal limit of Rs " << std::fixed << std::setprecision(2) << WITHDRAW_LIMIT << "\n";
        return false;
    }
    if (amount > balance) {
        std::cout << "[Error] Insufficient balance. Available: Rs " << std::fixed << std::setprecision(2) << balance << "\n";
        return false;
    }

    balance -= amount;
    txLog.push_back({accountNumber, getCurrentTimestamp(), "WITHDRAWAL", amount, balance});
    std::cout << "[Success] Withdrew Rs " << std::fixed << std::setprecision(2) << amount << ". Remaining: Rs " << balance << "\n";

    if (balance < MIN_BALANCE) {
        balance -= LOW_BAL_FEE;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "MIN_BAL_FEE", LOW_BAL_FEE, balance});
        std::cout << "[Notice] Minimum balance breached! Deducted fee of Rs " << LOW_BAL_FEE << ". Final: Rs " << balance << "\n";
    }
    return true;
}

void SavingsAccount::applyMonthlyCharges(std::vector<Transaction>& txLog) {
    if (balance < MIN_BALANCE) {
        balance -= LOW_BAL_FEE;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", LOW_BAL_FEE, balance});
        std::cout << ">> Acc " << accountNumber << " (Savings): Deducted Rs " << LOW_BAL_FEE << " for low balance.\n";
    }
}

// Current Account
CurrentAccount::CurrentAccount(int accNum, int custId, double initialDeposit)
    : Account(accNum, custId, initialDeposit, "CURRENT") {}

bool CurrentAccount::canWithdraw(double amount) const {
    return !((balance - amount) < -OVERDRAFT_LIMIT);
}

bool CurrentAccount::withdraw(double amount, std::vector<Transaction>& txLog) {
    if ((balance - amount) < -OVERDRAFT_LIMIT) {
        std::cout << "[Error] Overdraft limit breached! Maximum credit is Rs -" 
                  << std::fixed << std::setprecision(2) << OVERDRAFT_LIMIT << "\n";
        return false;
    }

    balance -= amount;
    txLog.push_back({accountNumber, getCurrentTimestamp(), "WITHDRAWAL", amount, balance});
    std::cout << "[Success] Withdrew Rs " << std::fixed << std::setprecision(2) << amount << ". Remaining: Rs " << balance << "\n";

    if (balance < MIN_BALANCE) {
        std::cout << "[Notice] Balance is below Rs 10000.00.\n";
    }
    return true;
}

void CurrentAccount::applyMonthlyCharges(std::vector<Transaction>& txLog) {
    if (balance < MIN_BALANCE) {
        balance -= MONTHLY_PENALTY;
        txLog.push_back({accountNumber, getCurrentTimestamp(), "MTH_PENALTY", MONTHLY_PENALTY, balance});
        std::cout << ">> Acc " << accountNumber << " (Current): Below minimum. Deducted Rs " 
                  << MONTHLY_PENALTY << " penalty.\n";
    }
}
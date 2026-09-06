#pragma once
#include "Models.h"
#include <string>
#include <vector>

class Account {
protected:
    int accountNumber;
    int customerId;
    double balance;
    std::string accountType;

public:
    Account();
    Account(int accNum, int custId, double initialDeposit, std::string accType);
    virtual ~Account() = default;

    int getAccountNumber() const;
    int getCustomerId() const;
    double getBalance() const;
    std::string getAccountType() const;

    void deposit(double amount);
    virtual bool canWithdraw(double amount) const = 0;
    virtual bool withdraw(double amount, std::vector<Transaction>& txLog) = 0;
    virtual void applyMonthlyCharges(std::vector<Transaction>& txLog) = 0;

    virtual std::string serialize() const;
    static Account* deserialize(const std::string& line);
};

class SavingsAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 500.0;
    static constexpr double LOW_BAL_FEE = 25.0;
    static constexpr double WITHDRAW_LIMIT = 20000.0;

    SavingsAccount(int accNum, int custId, double initialDeposit);
    bool canWithdraw(double amount) const override;
    bool withdraw(double amount, std::vector<Transaction>& txLog) override;
    void applyMonthlyCharges(std::vector<Transaction>& txLog) override;
};

class CurrentAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 10000.0;
    static constexpr double MONTHLY_PENALTY = 500.0;
    static constexpr double OVERDRAFT_LIMIT = 25000.0;

    CurrentAccount(int accNum, int custId, double initialDeposit);
    bool canWithdraw(double amount) const override;
    bool withdraw(double amount, std::vector<Transaction>& txLog) override;
    void applyMonthlyCharges(std::vector<Transaction>& txLog) override;
};
#pragma once
#include <string>
#include <vector>
#include <sstream>

struct AccountApplication {
    int appId;
    int customerId;
    std::string name;
    std::string mobile;
    std::string address;
    int age;
    std::string email;
    std::string accountType;
    double initialDeposit;
    size_t pinHash;
    std::string salt;
    std::string status;

    std::string serialize() const;
    static AccountApplication deserialize(const std::string& line);
};

struct Transaction {
    int accountNumber;
    std::string timestamp;
    std::string type;
    double amount;
    double balanceAfter;

    std::string serialize() const;
    static Transaction deserialize(const std::string& line);
};

struct FixedDeposit {
    int fdId;
    int accountNumber;
    std::string depositType;
    double principal;
    int tenureMonths;
    double interestRate;
    double maturityAmount;
    std::string creationDate;

    std::string serialize() const;
    static FixedDeposit deserialize(const std::string& line);
};
struct Loan {
    int loanId;
    int customerId;
    std::string loanType; // "PERSONAL", "HOME", "CREDIT_CARD"
    double principal;
    double interestRate;
    int tenureMonths;
    double monthlyEmi;
    double remainingBalance;
    std::string status;   // "ACTIVE", "PAID"

    std::string serialize() const;
    static Loan deserialize(const std::string& line);
    static double calculateEmi(double principal, double annualRate, int months);
};
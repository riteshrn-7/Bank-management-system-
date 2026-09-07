#include "Models.h"

std::string AccountApplication::serialize() const {
    return std::to_string(appId) + "|" +
           std::to_string(customerId) + "|" +
           name + "|" +
           mobile + "|" +
           address + "|" +
           std::to_string(age) + "|" +
           email + "|" +
           accountType + "|" +
           std::to_string(initialDeposit) + "|" +
           std::to_string(pinHash) + "|" +
           salt + "|" +
           status;
}

AccountApplication AccountApplication::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) {
        tokens.push_back(item);
    }

    if (tokens.size() >= 12) {
        try {
            return {
                std::stoi(tokens[0]),
                std::stoi(tokens[1]),
                tokens[2],
                tokens[3],
                tokens[4],
                std::stoi(tokens[5]),
                tokens[6],
                tokens[7],
                std::stod(tokens[8]),
                std::stoull(tokens[9]),
                tokens[10],
                tokens[11]
            };
        } catch (...) {
            return {0, 0, "", "", "", 0, "", "", 0.0, 0, "", ""};
        }
    } else if (tokens.size() == 11) { // Purana 11-token format fallback
        try {
            return {
                std::stoi(tokens[0]),
                std::stoi(tokens[1]),
                tokens[2],
                tokens[3],
                tokens[4],
                std::stoi(tokens[5]),
                tokens[6],
                tokens[7],
                std::stod(tokens[8]),
                std::stoull(tokens[9]),
                "",
                tokens[10]
            };
        } catch (...) {
            return {0, 0, "", "", "", 0, "", "", 0.0, 0, "", ""};
        }
    }
    return {0, 0, "", "", "", 0, "", "", 0.0, 0, "", ""};
}

std::string Transaction::serialize() const {
    return std::to_string(accountNumber) + "|" + timestamp + "|" + type + "|" + std::to_string(amount) + "|" + std::to_string(balanceAfter);
}

Transaction Transaction::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);
    if (tokens.size() == 5) {
        try {
            return {std::stoi(tokens[0]), tokens[1], tokens[2], std::stod(tokens[3]), std::stod(tokens[4])};
        } catch (...) {
            return {0, "", "", 0.0, 0.0};
        }
    }
    return {0, "", "", 0.0, 0.0};
}

std::string FixedDeposit::serialize() const {
    return std::to_string(fdId) + "|" + std::to_string(accountNumber) + "|" + depositType + "|" + 
           std::to_string(principal) + "|" + std::to_string(tenureMonths) + "|" + 
           std::to_string(interestRate) + "|" + std::to_string(maturityAmount) + "|" + creationDate;
}

FixedDeposit FixedDeposit::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);
    if (tokens.size() == 8) {
        try {
            return {std::stoi(tokens[0]), std::stoi(tokens[1]), tokens[2], std::stod(tokens[3]), 
                    std::stoi(tokens[4]), std::stod(tokens[5]), std::stod(tokens[6]), tokens[7]};
        } catch (...) {
            return {0, 0, "", 0.0, 0, 0.0, 0.0, ""};
        }
    }
    return {0, 0, "", 0.0, 0, 0.0, 0.0, ""};
}

#include <cmath>

double Loan::calculateEmi(double p, double annualRate, int months) {
    double r = (annualRate / 12.0) / 100.0;
    return (p * r * std::pow(1 + r, months)) / (std::pow(1 + r, months) - 1);
}

std::string Loan::serialize() const {
    return std::to_string(loanId) + "|" + std::to_string(customerId) + "|" + loanType + "|" +
           std::to_string(principal) + "|" + std::to_string(interestRate) + "|" +
           std::to_string(tenureMonths) + "|" + std::to_string(monthlyEmi) + "|" +
           std::to_string(remainingBalance) + "|" + status;
}

Loan Loan::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);

    if (tokens.size() == 9) {
        try {
            return {std::stoi(tokens[0]), std::stoi(tokens[1]), tokens[2],
                    std::stod(tokens[3]), std::stod(tokens[4]), std::stoi(tokens[5]),
                    std::stod(tokens[6]), std::stod(tokens[7]), tokens[8]};
        } catch (...) {
            return {0, 0, "", 0.0, 0.0, 0, 0.0, 0.0, "PAID"};
        }
    }
    return {0, 0, "", 0.0, 0.0, 0, 0.0, 0.0, "PAID"};
}
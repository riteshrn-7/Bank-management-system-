#include "Customer.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>

Customer::Customer(int id, std::string n, std::string m, std::string a, int ag, 
                   std::string e, size_t pHash, std::string s, 
                   bool permLocked, long long lUntil, int cycles)
    : customerId(id), name(n), mobile(m), address(a), age(ag), 
      email(e), pinHash(pHash), salt(s), isPermanentlyLocked(permLocked), 
      lockUntil(lUntil), failedAttempts(0), lockCycles(cycles) {
    if (salt.empty()) {
        salt = generateSalt();
    }
}

int Customer::getRemainingLockSeconds() const {
    if (isPermanentlyLocked) return -1;
    long long now = static_cast<long long>(std::time(nullptr));
    if (now < lockUntil) {
        return static_cast<int>(lockUntil - now);
    }
    return 0;
}

bool Customer::authenticate(const std::string& inputPin) {
    if (isPermanentlyLocked) return false;
    if (getRemainingLockSeconds() > 0) return false;

    // If cooldown passed, reset failedAttempts for the new cycle
    if (lockUntil != 0 && getRemainingLockSeconds() == 0) {
        failedAttempts = 0;
        lockUntil = 0;
    }

    if (computeSaltedHash(inputPin, salt) == pinHash) {
        failedAttempts = 0;
        lockCycles = 0;
        lockUntil = 0;
        return true;
    }

    failedAttempts++;
    if (failedAttempts >= 3) {
        lockCycles++;
        if (lockCycles >= 2) {
            // Second failure streak: Permanent lockout requiring Admin
            isPermanentlyLocked = true;
            lockUntil = 0;
        } else {
            // First failure streak: 60-second cooldown timer
            lockUntil = static_cast<long long>(std::time(nullptr)) + 60;
        }
    }
    return false;
}

void Customer::unlockByAdmin() {
    isPermanentlyLocked = false;
    lockUntil = 0;
    failedAttempts = 0;
    lockCycles = 0;
}

void Customer::displayProfile() const {
    std::cout << "Holder Name     : " << name << "\n";
    std::cout << "Mobile Number   : " << mobile << "\n";
    std::cout << "Email Address   : " << email << "\n";
    std::cout << "Billing Address : " << address << "\n";
}

std::string Customer::serialize() const {
    return std::to_string(customerId) + "|" + name + "|" + mobile + "|" +
           address + "|" + std::to_string(age) + "|" + email + "|" +
           std::to_string(pinHash) + "|" + salt + "|" + 
           (isPermanentlyLocked ? "1" : "0") + "|" + 
           std::to_string(lockUntil) + "|" + 
           std::to_string(lockCycles);
}

Customer Customer::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);
    
    if (tokens.size() >= 7) {
        std::string s = (tokens.size() >= 8) ? tokens[7] : generateSalt();
        bool permLocked = (tokens.size() >= 9) ? (tokens[8] == "1") : false;
        long long lUntil = (tokens.size() >= 10) ? std::stoll(tokens[9]) : 0;
        int cycles = (tokens.size() >= 11) ? std::stoi(tokens[10]) : 0;

        return Customer(std::stoi(tokens[0]), tokens[1], tokens[2], tokens[3],
                        std::stoi(tokens[4]), tokens[5], std::stoull(tokens[6]), 
                        s, permLocked, lUntil, cycles);
    }
    return Customer(0, "", "", "", 0, "", 0, "");
}
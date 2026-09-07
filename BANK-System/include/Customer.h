#pragma once
#include <string>
#include <ctime>

class Customer {
private:
    int customerId;
    std::string name;
    std::string mobile;
    std::string address;
    int age;
    std::string email;
    size_t pinHash;
    std::string salt;
    bool isPermanentlyLocked;
    long long lockUntil;
    int failedAttempts;
    int lockCycles;

public:
    Customer(int id, std::string n, std::string m, std::string a, int ag, 
             std::string e, size_t pHash, std::string s = "", 
             bool permLocked = false, long long lUntil = 0, int cycles = 0);

    // Getters
    int getCustomerId() const { return customerId; }
    std::string getName() const { return name; }
    std::string getMobile() const { return mobile; }
    std::string getAddress() const { return address; }
    int getAge() const { return age; }
    std::string getEmail() const { return email; }
    size_t getPinHash() const { return pinHash; }
    std::string getSalt() const { return salt; }

    // Lockout Status
    bool isLockedPermanently() const { return isPermanentlyLocked; }
    int getRemainingLockSeconds() const;

    // Setters
    void setMobile(const std::string& m) { mobile = m; }
    void setAddress(const std::string& a) { address = a; }
    void setEmail(const std::string& e) { email = e; }

    // Authentication & Admin Unlock
    bool authenticate(const std::string& inputPin);
    bool verifyPin(const std::string& pin) { return authenticate(pin); }
    void unlockByAdmin();

    // UI & Storage
    void displayProfile() const;
    std::string serialize() const;
    static Customer deserialize(const std::string& line);
};
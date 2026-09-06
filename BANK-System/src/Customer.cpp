#include "Customer.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <functional>

Customer::Customer() : customerId(0), name(""), mobile(""), address(""), age(0), email(""), customerPinHash(0) {}

Customer::Customer(int id, std::string n, std::string m, std::string a, int ag, std::string e, size_t pinH)
    : customerId(id), name(n), mobile(m), address(a), age(ag), email(e), customerPinHash(pinH) {}

int Customer::getCustomerId() const { return customerId; }
std::string Customer::getName() const { return name; }
std::string Customer::getMobile() const { return mobile; }
std::string Customer::getAddress() const { return address; }
int Customer::getAge() const { return age; }
std::string Customer::getEmail() const { return email; }

bool Customer::verifyPin(const std::string& enteredPin) const {
    std::hash<std::string> hasher;
    return hasher(enteredPin) == customerPinHash;
}

void Customer::setMobile(const std::string& m) { mobile = m; }
void Customer::setAddress(const std::string& a) { address = a; }
void Customer::setEmail(const std::string& e) { email = e; }

void Customer::displayProfile() const {
    std::cout << "\n--- Customer Profile ---\n";
    std::cout << "Customer ID : " << customerId << "\n";
    std::cout << "Name        : " << name << "\n";
    std::cout << "Mobile      : " << mobile << "\n";
    std::cout << "Address     : " << address << "\n";
    std::cout << "Age         : " << age << "\n";
    std::cout << "Email       : " << email << "\n";
}

std::string Customer::serialize() const {
    return std::to_string(customerId) + "|" + name + "|" + mobile + "|" + address + "|" + 
           std::to_string(age) + "|" + email + "|" + std::to_string(customerPinHash);
}

Customer Customer::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, '|')) tokens.push_back(item);
    if (tokens.size() == 7) {
        try {
            return Customer(std::stoi(tokens[0]), tokens[1], tokens[2], tokens[3], 
                            std::stoi(tokens[4]), tokens[5], std::stoull(tokens[6]));
        } catch (...) {
            return Customer();
        }
    }
    return Customer();
}
#pragma once
#include <string>

class Customer {
private:
    int customerId;
    std::string name;
    std::string mobile;
    std::string address;
    int age;
    std::string email;
    size_t customerPinHash;

public:
    Customer();
    Customer(int id, std::string n, std::string m, std::string a, int ag, std::string e, size_t pinH);

    int getCustomerId() const;
    std::string getName() const;
    std::string getMobile() const;
    std::string getAddress() const;
    int getAge() const;
    std::string getEmail() const;

    bool verifyPin(const std::string& enteredPin) const;
    void setMobile(const std::string& m);
    void setAddress(const std::string& a);
    void setEmail(const std::string& e);
    void displayProfile() const;

    std::string serialize() const;
    static Customer deserialize(const std::string& line);
};
#pragma once
#include "Customer.h"
#include "Account.h"
#include "Models.h"
#include <vector>
#include <string>

class BankSystem {
private:
    std::vector<Customer> customers;
    std::vector<Account*> accounts;
    std::vector<Transaction> transactions;
    std::vector<FixedDeposit> deposits;
    std::vector<AccountApplication> applications;

    const std::string customerFile = "customers.txt";
    const std::string accountFile = "accounts.txt";
    const std::string transactionFile = "transactions.txt";
    const std::string fdFile = "fds.txt";
    const std::string appFile = "applications.txt";
    const std::string adminPin = "admin123";

    void clearBuffer();
    Customer* findCustomer(int id);
    Account* findAccount(int accNum);
    std::vector<Account*> getAccountsForCustomer(int custId);
    bool authenticateCustomer(Customer* cust);

    void loadData();
    void saveData();

    void manageSingleAccount(Account* acc, Customer* cust);
    void executeInternalTransfer(const std::vector<Account*>& myAccounts);
    void deposit(Account* acc);
    void withdraw(Account* acc);
    void transferFunds(Account* sender);
    void createDeposit(Account* acc);
    void viewCustomerDeposits(int accNum);
    void exportStatementCSV(Account* acc, Customer* cust);
    void displayAccountDetails(Account* acc, Customer* cust);

    int countPendingApplications();
    void processApplications();
    void directAdminOnboarding();
    void viewBankReserves();
    void listAllAccounts();
    void updateCustomer();
    void runMonthlyMaintenanceCycle();
    void inspectMasterStatement();

public:
    BankSystem();
    ~BankSystem();

    void applyForAccount();
    void customerPortal();
    void adminPortal();
};
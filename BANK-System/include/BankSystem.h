#pragma once
#include "Customer.h"
#include "Account.h"
#include "Models.h"
#include <vector>
#include <string>

class BankSystem {
private:
    const std::string customerFile = "customers.txt";
    const std::string accountFile = "accounts.txt";
    const std::string transactionFile = "transactions.txt";
    const std::string fdFile = "fds.txt";
    const std::string appFile = "applications.txt";
    const std::string loanFile = "loans.txt";
    const std::string adminPin = "admin123";

    std::vector<Customer> customers;
    std::vector<Account*> accounts;
    std::vector<Transaction> transactions;
    std::vector<FixedDeposit> deposits;
    std::vector<AccountApplication> applications;
    std::vector<Loan> loans;

    // Helpers
    void clearBuffer();
    Customer* findCustomer(int id);
    Account* findAccount(int accNum);
    std::vector<Account*> getAccountsForCustomer(int custId);
    bool authenticateCustomer(Customer* cust);

    // Account Actions
    void manageSingleAccount(Account* acc, Customer* cust);
    void executeInternalTransfer(const std::vector<Account*>& myAccounts);
    void deposit(Account* acc);
    void withdraw(Account* acc);
    void transferFunds(Account* sender);
    void createDeposit(Account* acc);
    void viewCustomerDeposits(int accNum);
    void exportStatementCSV(Account* acc, Customer* cust);
    void displayAccountDetails(Account* acc, Customer* cust);

    // Admin Panel Actions
    int countPendingApplications();
    void processApplications();
    void viewBankReserves();
    void listAllAccounts();
    void updateCustomer();
    void runMonthlyMaintenanceCycle();
    void inspectMasterStatement();
    void directAdminOnboarding();
    void unlockCustomerAccount();

    // Loan Operations
    void applyForLoan(Customer* cust);

public:
    BankSystem();
    ~BankSystem();

    void loadData();
    void saveData();

    void applyForAccount();
    void customerPortal();
    void adminPortal();
};
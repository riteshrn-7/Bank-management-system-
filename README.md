# BasicMicro-Projects-c++
# This repository contains my basic C++ projects and practice programs as a Diploma Computer Engineering student. 
#I am developing my programming skills through practical coding and learning concepts.
#I will keep updating it as I learn and improve.


# 🏦 Bank Management System

A robust, resume-ready console banking application built with **C++17**, featuring an industry-standard modular multi-file architecture, object-oriented design, file-based persistence, and role-based security controls.

---

## ✨ Features

- **Relational 1:N Account Mapping**: Users log in using a single Customer ID and can manage multiple linked accounts (Savings, Current).
- **Role-Based Portals**:
  - **Customer Portal**: Check balances, make deposits/withdrawals, initiate internal/third-party transfers, and open term deposits.
  - **Admin/Manager Portal**: Review and approve/reject account applications, audit liquid/locked bank reserves, and run monthly maintenance fee cycles.
- **Account Application Pipeline**: Prospective users submit applications from the main gateway; requests remain in `PENDING` status until an Admin reviews and approves them.
- **Security & PIN Hashing**: Customer 4-digit PINs are securely hashed via `std::hash<std::string>` before storage; plain text PINs are never stored.
- **Atomic Fund Transfers**: Pre-transfer validation ensures funds and overdraft limits clear before committing changes across sender and receiver accounts.
- **Term Deposit Engine (FD / RD)**: Computes quarterly compound interest returns based on investment tenure:
  $$\text{Maturity} = P \times \left(1 + \frac{r}{400}\right)^{4t}$$
- **CSV Statement Exporter**: Exports detailed transaction ledgers directly into Excel-compatible `.csv` files (`statement_<AccNo>.csv`).

---

## 📁 Project Structure

```text
BANK-System/
├── include/               # Header interfaces (.h)
│   ├── Account.h          # Abstract base Account, SavingsAccount, CurrentAccount
│   ├── BankSystem.h       # System controller & portal operations
│   ├── Customer.h         # Customer profile model & authentication
│   ├── Models.h           # Data transfer structs (Transaction, FD, Applications)
│   └── Utils.h            # Timestamp utility declarations
│
├── src/                   # Implementation files (.cpp)
│   ├── Account.cpp        # Withdrawal rules, overdraft limits, penalty logic
│   ├── BankSystem.cpp     # Controller operations, file I/O, UI menus
│   ├── Customer.cpp       # Profile mutation & PIN validation logic
│   ├── Models.cpp         # Custom pipe serialization / deserialization
│   └── Utils.cpp          # Formatted system time generator
```
## admin pass 
admin123
│
├── main.cpp               # Gateway entry point
├── README.md              # Project documentation
└── *.txt                  # Flat-file databases (auto-generated)

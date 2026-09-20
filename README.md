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
│
└── *.txt                  # Flat-file databases (auto-generated) 

🛠️ Tech Stack & Prerequisites
Language: C++11 / C++17

Compiler: g++ (Linux/macOS) or mingw-w64 (Windows)

Standard Libraries: <iostream>, <fstream>, <vector>, <thread>, <chrono>, <cmath>

Database: Local .txt flat-file storage (State is persisted automatically).

🚀 Installation & Setup Instructions
Ensure your terminal is navigated to the root BANK-System/ directory where main.cpp is located.

Option 1: Linux / macOS (Native Compilation)
Bash
# Compile the binary
g++ main.cpp src/BankSystem.cpp src/Account.cpp src/Customer.cpp src/Models.cpp src/Utils.cpp -I include -o bank_app

# Execute the application
./bank_app
Option 2: Windows (MinGW Cross-Compilation)
If you are developing in Linux/WSL but need a native .exe for Windows, use static linking to prevent missing DLL errors:

Bash
# Compile the Windows Executable
x86_64-w64-mingw32-g++ main.cpp src/BankSystem.cpp src/Account.cpp src/Customer.cpp src/Models.cpp src/Utils.cpp -I include -static -static-libgcc -static-libstdc++ -o bank_app.exe

# Execute the application (in CMD/PowerShell)
./bank_app.exe
(Note: If compiling directly inside a Windows IDE like Visual Studio or Code::Blocks, simply import all source files and click "Build & Run".)

🛡️ Default Credentials
To test administrative approval flows and audits, use the default manager key:

Portal: Admin / Manager Portal

Master Admin Key: 1234 (Configurable in BankSystem.cpp constructor)

#include "BankSystem.h"
#include <iostream>
#include <limits>

using namespace std;

int main() {
    BankSystem system;
    int choice = 0;

    while (choice != 4) {
        cout << "\n=========================================\n";
        cout << "       CENTRAL SECURE BANKING GATEWAY    \n";
        cout << "=========================================\n";
        cout << "1. Customer Portal (Login with Customer ID)\n";
        cout << "2. Apply for New Account / Registration\n";
        cout << "3. Admin / Manager Portal (Approvals & Audits)\n";
        cout << "4. Exit System\n";
        cout << "Select Gateway (1-4): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.customerPortal(); break;
            case 2: system.applyForAccount(); break;
            case 3: system.adminPortal(); break;
            case 4: cout << "Securing database and shutting down. Goodbye!\n"; break;
            default: cout << "Invalid selection.\n"; break;
        }
    }
    return 0;
}
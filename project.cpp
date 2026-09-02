#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// ============================================================
// Utility Functions
// ============================================================

string trim(const string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");

    if (start == string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t\r\n");

    return str.substr(start, end - start + 1);
}

// Validate customer name
bool isValidName(const string& name)
{
    if (name.empty() || name.length() > 50)
        return false;

    for (size_t i = 0; i < name.length(); ++i)
    {
        unsigned char ch = static_cast<unsigned char>(name[i]);

        if (!isalpha(ch) &&
            name[i] != ' ' &&
            name[i] != '-' &&
            name[i] != '\'')
        {
            return false;
        }
    }

    return true;
}

// Convert money to cents.
// Example:
// "100"    -> 10000
// "100.50" -> 10050
bool parseMoney(const string& input, long long& cents)
{
    string value = trim(input);

    if (value.empty() || value[0] == '-')
        return false;

    size_t dot = value.find('.');

    // More than one decimal point
    if (dot != string::npos &&
        value.find('.', dot + 1) != string::npos)
    {
        return false;
    }

    string whole;
    string fraction;

    if (dot == string::npos)
    {
        whole = value;
        fraction = "00";
    }
    else
    {
        whole = value.substr(0, dot);
        fraction = value.substr(dot + 1);

        if (fraction.length() > 2)
            return false;

        if (fraction.empty())
            fraction = "00";
        else if (fraction.length() == 1)
            fraction += "0";
    }

    if (whole.empty())
        whole = "0";

    // Validate whole-number part
    for (size_t i = 0; i < whole.length(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(whole[i])))
            return false;
    }

    // Validate decimal part
    for (size_t i = 0; i < fraction.length(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(fraction[i])))
            return false;
    }

    try
    {
        long long dollars = stoll(whole);
        long long decimal = stoll(fraction);

        if (dollars >
            (numeric_limits<long long>::max() - decimal) / 100)
        {
            return false;
        }

        cents = dollars * 100 + decimal;

        return cents > 0;
    }
    catch (...)
    {
        return false;
    }
}

// Convert cents to formatted currency
string moneyToString(long long paise)
{
    ostringstream out;

    out << "Rs. "
        << paise / 100
        << "."
        << setw(2)
        << setfill('0')
        << paise % 100;

    return out.str();
}

// ============================================================
// Customer Class
// ============================================================

class Customer
{
private:
    long long accountNumber;
    string name;
    long long balance;

public:

    Customer()
    {
        accountNumber = 0;
        name = "";
        balance = 0;
    }

    Customer(long long accountNumber,
             const string& name,
             long long balance = 0)
    {
        this->accountNumber = accountNumber;
        this->name = name;
        this->balance = balance;
    }

    long long getAccountNumber() const
    {
        return accountNumber;
    }

    string getName() const
    {
        return name;
    }

    long long getBalance() const
    {
        return balance;
    }

    bool deposit(long long amount)
    {
        if (amount <= 0)
            return false;

        if (balance >
            numeric_limits<long long>::max() - amount)
        {
            return false;
        }

        balance += amount;

        return true;
    }

    bool withdraw(long long amount)
    {
        if (amount <= 0 || amount > balance)
            return false;

        balance -= amount;

        return true;
    }

    void display() const
    {
        cout << "\n--------------------------------\n";
        cout << "Account Number : " << accountNumber << endl;
        cout << "Customer Name  : " << name << endl;
        cout << "Balance        : " << moneyToString(balance) << endl;
        cout << "--------------------------------\n";
    }
};

// ============================================================
// Bank Class
// ============================================================

class Bank
{
private:
    unordered_map<long long, Customer> customers;

    const string dataFile;
    const string tempFile;

    long long nextAccountNumber;

    // --------------------------------------------------------
    // Generate unique account number
    // --------------------------------------------------------

    long long generateAccountNumber()
    {
        while (customers.find(nextAccountNumber) != customers.end())
        {
            ++nextAccountNumber;
        }

        return nextAccountNumber++;
    }

    // --------------------------------------------------------
    // Save data to file
    // --------------------------------------------------------

    bool save() const
    {
        ofstream file(tempFile.c_str(), ios::out | ios::trunc);

        if (!file)
            return false;

        unordered_map<long long, Customer>::const_iterator it;

        for (it = customers.begin(); it != customers.end(); ++it)
        {
            const Customer& customer = it->second;

            file << customer.getAccountNumber()
                 << "|"
                 << customer.getName()
                 << "|"
                 << customer.getBalance()
                 << "\n";
        }

        file.close();

        if (!file)
            return false;

        // Delete old database
        remove(dataFile.c_str());

        // Rename temporary file
        if (rename(tempFile.c_str(), dataFile.c_str()) != 0)
        {
            remove(tempFile.c_str());
            return false;
        }

        return true;
    }

public:

    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    Bank()
        : dataFile("customers.dat"),
          tempFile("customers.tmp"),
          nextAccountNumber(100001)
    {
    }

    // --------------------------------------------------------
    // Load data from file
    // --------------------------------------------------------

    bool load()
    {
        ifstream file(dataFile.c_str());

        if (!file)
        {
            // File doesn't exist.
            // This is normal on first execution.
            return true;
        }

        // Important:
        // Clear existing records before loading.
        customers.clear();

        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            string accountString;
            string name;
            string balanceString;

            stringstream ss(line);

            if (!getline(ss, accountString, '|'))
                continue;

            if (!getline(ss, name, '|'))
                continue;

            if (!getline(ss, balanceString))
                continue;

            try
            {
                long long accountNumber =
                    stoll(accountString);

                long long balance =
                    stoll(balanceString);

                name = trim(name);

                if (accountNumber <= 0 ||
                    balance < 0 ||
                    !isValidName(name))
                {
                    continue;
                }

                Customer customer(
                    accountNumber,
                    name,
                    balance
                );

                customers[accountNumber] = customer;

                if (accountNumber >= nextAccountNumber)
                {
                    nextAccountNumber =
                        accountNumber + 1;
                }
            }
            catch (...)
            {
                // Ignore corrupted records.
            }
        }

        file.close();

        return true;
    }

    // --------------------------------------------------------
    // Find customer
    // --------------------------------------------------------

    Customer* findCustomer(long long accountNumber)
    {
        unordered_map<long long, Customer>::iterator it =
            customers.find(accountNumber);

        if (it == customers.end())
            return NULL;

        return &it->second;
    }

    // --------------------------------------------------------
    // Create Account
    // --------------------------------------------------------

    void createAccount()
    {
        string name;

        cout << "\nEnter customer name: ";
        getline(cin, name);

        name = trim(name);

        if (!isValidName(name))
        {
            cout << "Invalid name.\n";
            return;
        }

        long long accountNumber =
            generateAccountNumber();

        customers.insert(
            make_pair(
                accountNumber,
                Customer(accountNumber, name, 0)
            )
        );

        if (!save())
        {
            customers.erase(accountNumber);

            cout << "Unable to save account.\n";
            return;
        }

        cout << "\nAccount created successfully.\n";
        cout << "Account Number: "
             << accountNumber
             << "\n";
    }

    // --------------------------------------------------------
    // Deposit
    // --------------------------------------------------------

    void depositMoney()
    {
        long long accountNumber;

        cout << "\nEnter account number: " << flush;

        if (!(cin >> accountNumber))
        {
            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "Invalid account number.\n";
            return;
        }

        Customer* customer =
            findCustomer(accountNumber);

        if (customer == NULL)
        {
            cout << "Account not found.\n";
            return;
        }

        string input;
        long long amount;

        cout << "Enter deposit amount: Rs." << flush;
        cin >> input;

        if (!parseMoney(input, amount))
        {
            cout << "Invalid amount.\n";
            return;
        }

        // Store old balance in case saving fails
        long long oldBalance =
            customer->getBalance();

        if (!customer->deposit(amount))
        {
            cout << "Deposit failed.\n";
            return;
        }

        if (!save())
        {
            // Restore old balance
            customer->withdraw(
                customer->getBalance() - oldBalance
            );

            cout << "Transaction could not be saved.\n";
            return;
        }

        cout << "\nDeposit successful.\n";
        cout << "New balance: "
             << moneyToString(customer->getBalance())
             << "\n";
    }

    // --------------------------------------------------------
    // Withdraw
    // --------------------------------------------------------

    void withdrawMoney()
    {
        long long accountNumber;

        cout << "\nEnter account number: ";

        if (!(cin >> accountNumber))
        {
            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "Invalid account number.\n";
            return;
        }

        Customer* customer =
            findCustomer(accountNumber);

        if (customer == NULL)
        {
            cout << "Account not found.\n";
            return;
        }

        string input;
        long long amount;

        cout << "Enter withdrawal amount: Rs.";
        cin >> input;

        if (!parseMoney(input, amount))
        {
            cout << "Invalid amount.\n";
            return;
        }

        // Store old balance in case saving fails
        long long oldBalance =
            customer->getBalance();

        if (!customer->withdraw(amount))
        {
            cout << "Insufficient balance or invalid amount.\n";
            return;
        }

        if (!save())
        {
            // Restore old balance
            customer->deposit(
                oldBalance - customer->getBalance()
            );

            cout << "Transaction could not be saved.\n";
            return;
        }

        cout << "\nWithdrawal successful.\n";
        cout << "New balance: "
             << moneyToString(customer->getBalance())
             << "\n";
    }

    // --------------------------------------------------------
    // Check Balance
    // --------------------------------------------------------

    void checkBalance()
    {
        long long accountNumber;

        cout << "\nEnter account number: ";

        if (!(cin >> accountNumber))
        {
            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "Invalid account number.\n";
            return;
        }

        Customer* customer =
            findCustomer(accountNumber);

        if (customer == NULL)
        {
            cout << "Account not found.\n";
            return;
        }

        customer->display();
    }

    // --------------------------------------------------------
    // Display All Accounts
    // --------------------------------------------------------

    void displayAllAccounts() const
    {
        if (customers.empty())
        {
            cout << "\nNo accounts found.\n";
            return;
        }

        vector<Customer> accounts;

        accounts.reserve(customers.size());

        unordered_map<long long, Customer>::const_iterator it;

        for (it = customers.begin();
             it != customers.end();
             ++it)
        {
            accounts.push_back(it->second);
        }

        sort(
            accounts.begin(),
            accounts.end(),
            [](const Customer& a, const Customer& b)
            {
                return a.getAccountNumber() <
                       b.getAccountNumber();
            }
        );

        cout << "\n========================================\n";
        cout << "          ALL CUSTOMER ACCOUNTS\n";
        cout << "========================================\n";

        for (size_t i = 0; i < accounts.size(); ++i)
        {
            accounts[i].display();
        }
    }
};

// ============================================================
// Main Function
// ============================================================

int main()
{
    Bank bank;

    if (!bank.load())
    {
        cerr << "Failed to load customer database.\n";
        return 1;
    }

    while (true)
    {
        cout << "\n";
        cout << "========================================\n";
        cout << "       BANK MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance\n";
        cout << "5. Display All Accounts\n";
        cout << "6. Exit\n";
        cout << "========================================\n";
        cout << "Enter choice: " << flush;

        int choice;

        if (!(cin >> choice))
        {
            cin.clear();

            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "Invalid choice.\n";
            continue;
        }

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        switch (choice)
        {
            case 1:
                bank.createAccount();
                break;

            case 2:
                bank.depositMoney();
                break;

            case 3:
                bank.withdrawMoney();
                break;

            case 4:
                bank.checkBalance();
                break;

            case 5:
                bank.displayAllAccounts();
                break;

            case 6:
                cout << "\nExiting Bank Management System...\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
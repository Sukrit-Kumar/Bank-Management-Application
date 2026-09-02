# Bank Management Application

A simple and efficient **C++ console-based Bank Management System** that simulates basic banking operations using **Object-Oriented Programming (OOP)** and **file handling**.

The project is designed as a learning project to demonstrate practical implementation of classes, encapsulation, data structures, file management, input validation, and persistent customer records in C++.

---

## Features

- Create new customer accounts
- Automatically generate unique account numbers
- Deposit money into an account
- Withdraw money from an account
- Check account balance
- Display all customer accounts
- Persistent customer data storage
- Input validation
- Insufficient balance protection
- Integer-based currency handling to avoid floating-point errors
- Efficient account lookup using `unordered_map`
- Temporary-file based data saving
- Compatible with older C++ compilers

---

## Technologies Used

- **Language:** C++
- **Standard:** C++11
- **Compiler:** GCC / MinGW
- **Compiler Version Tested:** GCC 6.3.0
- **Data Storage:** File handling
- **Data Structure:** `unordered_map`
- **Development Environment:** Visual Studio Code

---

## Object-Oriented Programming Concepts

This project demonstrates several important OOP concepts:

### Encapsulation

Customer information such as account number, name, and balance is kept private inside the `Customer` class.

### Abstraction

The `Bank` class provides functions for banking operations without exposing the internal implementation.

### Classes and Objects

The project uses separate classes for managing customers and banking operations.

### Member Functions

Operations such as:

- Deposit
- Withdrawal
- Balance checking
- Account creation

are implemented through class member functions.

---

## Banking Operations

The application provides the following menu:

```text
========================================
       BANK MANAGEMENT SYSTEM
========================================
1. Create Account
2. Deposit Money
3. Withdraw Money
4. Check Balance
5. Display All Accounts
6. Exit
========================================
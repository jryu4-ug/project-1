#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <memory> // For unique_ptr

using namespace std;

// Bank account structure
struct Account {
    int id;
    double balance;
    mutex lock;

    Account(int id, double balance) {
        this -> id = id;
        this -> balance = balance;
    }

    // Mutex cannot be copied or moved
    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;
};

// Global variables
vector<unique_ptr<Account>> accounts;
random_device rd;
mt19937 gen(rd());

// Function simulating bank deposit
void deposit(int accountId, double amount) {

    // Find the account and lock it
    for (auto & account : accounts) {
        if (account->id == accountId) {
            lock_guard<mutex> guard(account->lock);
            cout << "Thread " << this_thread::get_id() << " depositing $" << amount << " to account " << accountId << endl;

            // Simulate processing time
            this_thread::sleep_for(chrono::milliseconds(50));

            account->balance += amount;
            cout << "New balance for account " << accountId << ": $" << account->balance << endl;
            break;
        }
    }
}

// Function simulating bank withdrawal
void withdraw(int accountId, double amount) {

    // Find the account and lock it
    for (auto& account : accounts) {
        if (account->id == accountId) {
            lock_guard<mutex> guard(account->lock);
            cout << "Thread " << this_thread::get_id() << " withdrawing $" << amount << " from account " << accountId << endl;

            // Simulate processing time
            this_thread::sleep_for(chrono::milliseconds(50));

            if (account->balance >= amount) {
                account->balance -= amount;
                cout << "New balance for account " << accountId << ": $" << account->balance << endl;
            } else {
                cout << "Insufficient funds in account " << accountId << endl;
            }
            break;
        }
    }
}

// Function transferring money between accounts (possible deadlock)
void transfer(int fromAccountId, int toAccountId, double amount) {

    // Find both accounts
    Account* from = nullptr;
    Account* to = nullptr;

    for (auto& account : accounts) {
        if (account->id == fromAccountId) from = account.get();
        if (account->id == toAccountId) to = account.get();
    }

    if (!from || !to) {
        cout << "Invalid account IDs for transfer";
        return;
    }

    // Lock both accounts
    cout << "Thread " << this_thread::get_id() << " transferring $" << amount << " from account " << fromAccountId << " to account " << toAccountId << endl;

    from->lock.lock();
    cout << "Thread " << this_thread::get_id() << " locked account " << fromAccountId << endl;

    // Simulate processing delay, may cause deadlock
    this_thread::sleep_for(chrono::milliseconds(100));

    to->lock.lock();
    cout << "Thread " << this_thread::get_id() << " locked account " << toAccountId << endl;

    // Perform transfer
    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
        cout << "Transfer successful!" << endl;
    } else {
        cout << "Insufficient funds for transfer" << endl;
    }

    // Unlock in reverse order
    to->lock.unlock();
    from->lock.unlock();
}

// Function transferring money with deadlock prevention
void safeTransfer(int fromAccountId, int toAccountId, double amount) {

    // Locking accounts in order of ID (prevents deadlock)
    int firstLock = min(fromAccountId, toAccountId);
    int secondLock = max(fromAccountId, toAccountId);

    Account* from = nullptr;
    Account* to = nullptr;
    Account* first = nullptr;
    Account* second = nullptr;

    for (auto& account : accounts) {
        if (account->id == fromAccountId) from = account.get();
        if (account->id == toAccountId) to = account.get();
        if (account->id == firstLock) first = account.get();
        if (account->id == secondLock) second = account.get();
    }

    if (!from || !to) {
        cout << "Invalid account IDs for safe transfer" << endl;
        return;
    }

    cout << "Thread " << this_thread::get_id() << " safely transferring $" << amount << " from account " << fromAccountId << " to account " << toAccountId << endl;

    // Lock in the same order by ID
    first->lock.lock();
    cout << "Thread " << this_thread::get_id() << " locked account " << first->id << endl;

    // Simulate a delay, deadlock won't occur due to lock order
    this_thread::sleep_for(chrono::milliseconds(100));

    second->lock.lock();
    cout << "Thread " << this_thread::get_id() << " locked account " << second->id << endl;

    // Perform transfer
    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
        cout << "Safe transfer successful!" << endl;
    } else {
        cout << "Insufficient funds for safe transfer" << endl;
    }

    // Unlock in reverse order
    second->lock.unlock();
    first->lock.unlock();
}

// Basic thread operations
void phase1() {

    // Create 5 bank accounts
    accounts.clear();
    for (int i = 1; i <= 5; i++) {
        accounts.push_back(make_unique<Account>(i, 1000.0));
    }

    vector<thread> threads;

    // Create 5 deposit threads and 5 withdraw threads
    for (int i = 0; i < 5; i++) {
        int accountId = (i % 5) + 1;
        double amount = 100.0 + i * 10;

        // Deposit thread
        threads.push_back(thread(deposit, accountId, amount));

        // Withdraw thread
        threads.push_back(thread(withdraw, accountId, amount / 2));
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    // Display the final balances
    cout << "\nFinal Account Balances:" << endl;
    for (const auto& account : accounts) {
        cout << "Account " << account->id << ": $" << account->balance << endl;
    }
}

// Resource protection
void phase2() {

    // Reset accounts
    accounts.clear();
    for (int i = 1; i <= 5; i++) {
        accounts.push_back(make_unique<Account>(i, 1000.0));
    }

    vector<thread> threads;

    // Create threads that access the same accounts
    for (int i = 0; i < 10; i++) {
        int accountId = (i % 3) + 1; // Threads overlap on accounts 1, 2, 3
        double amount = 50.0 + i * 5;

        if (i % 2 == 0) {
            threads.push_back(thread(deposit, accountId, amount));
        } else {
            threads.push_back(thread(withdraw, accountId, amount / 2));
        }
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    // Display final balances
    cout << "\nFinal Account Balances:" << endl;
    for (const auto& account : accounts) {
        cout << "Account " << account->id << ": $" << account->balance << endl;
    }
}

// Deadlock creation
void phase3() {

    // Reset accounts
    accounts.clear();
    for (int i = 1; i <= 5; i++) {
        accounts.push_back(make_unique<Account>(i, 1000.0));
    }

    // Create threads that may deadlock
    thread t1(transfer, 1, 2, 300.0);
    thread t2(transfer, 2, 1, 200.0);

    // Wait for threads
    t1.join();
    t2.join();

    // Display final balances
    cout << "\nFinal Account Balances:" << endl;
    for (const auto& account : accounts) {
        cout << "Account " << account->id << ": $" << account->balance << endl;
    }
}

// Deadlock resolution
void phase4() {

    // Reset accounts
    accounts.clear();
    for (int i = 1; i <= 5; i++) {
        accounts.push_back(make_unique<Account>(i, 1000.0));
    }

    // Create threads with deadlock prevention
    thread t1(safeTransfer, 1, 2, 300.0);
    thread t2(safeTransfer, 2, 1, 200.0);

    // Wait for threads
    t1.join();
    t2.join();

    // Display final balances
    cout << "\nFinal Account Balances:" << endl;
    for (const auto& account : accounts) {
        cout << "Account " << account->id << ": $" << account->balance << endl;
    }
}

int main() {
    // Basic Thread Operations
    // Bank deposit and withdrawal
    //phase1();

    // Resource Protection
    // Protected account access
    // phase2();

    // Deadlock Creation
    // phase3();

    // Deadlock Resolution
    // phase4();
}
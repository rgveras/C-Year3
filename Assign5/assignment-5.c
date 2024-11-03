//
// Created by Ricardo Veras on 2024-03-21.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_ACCOUNTS 4
#define NUM_THREADS 4
#define MAX_TRANSACTIONS 50

// Account struct
typedef struct {
    int balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];     // Initialize 4 account structures

// Transaction struct
typedef struct {
    int accountNum;
    int amount;
} Transaction;

// ThreadData struct
typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int numTransactions;
} ThreadData;

ThreadData threadData[NUM_THREADS];     // Initialize 4 threadData structures

// ThreadParams struct. Contains thread ID and transaction info
typedef struct {
    int threadID;
    ThreadData* data;
} ThreadParams;

// Process transactions for a given thread
void *processTransactions(void *arg) {
    ThreadParams* params = (ThreadParams*)arg;
    ThreadData* data = params->data;
    int threadID = params->threadID;

    for (int i = 0; i < data->numTransactions; i++) {
        int accountNum = data->transactions[i].accountNum - 1; // Adjust for 0-based indexing
        int amount = data->transactions[i].amount;

        pthread_mutex_lock(&accounts[accountNum].lock);     // Lock account before processing transaction

        // If withdrawal amount exceeds balance, print insufficient funds message, otherwise withdraw/deposit amount and print transaction information
        if (amount < 0 && accounts[accountNum].balance + amount < 0) {
            printf("Thread %d:\n    Withdraw $%d from Account %d\n    *** INSUFFICIENT FUNDS ***\n    Account %d: $%d\n", threadID + 1, abs(amount), accountNum + 1, accountNum + 1, accounts[accountNum].balance);
        } else {
            accounts[accountNum].balance += amount;
            printf("Thread %d:\n    %s $%d %s Account %d\n    Account %d: $%d\n", threadID + 1, amount > 0 ? "Deposit" : "Withdraw", abs(amount), amount > 0 ? "to" : "from", accountNum + 1, accountNum + 1, accounts[accountNum].balance);
        }
        pthread_mutex_unlock(&accounts[accountNum].lock);   // Unlock account once transaction is completed
    }
    pthread_exit(NULL);     // Exit process
}

// Accept a filename argument with bank account balances and transactions, process transactions and update necessary information
int main(int argc, char *argv[]) {

    // If incorrect number of arguments, display argument format
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open file, print error message if it was not opened correctly
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Initialize accounts and mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        fscanf(file, "%d", &accounts[i].balance);
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    // Print starting balance of each account
    printf("Balance\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("    Account %d: $%d\n", i + 1, accounts[i].balance);
    }

    // Read the file and prepare thread data
    char line[256];
    fgets(line, sizeof(line), file); // Skip the first line with account balances

    // For each line, assign transaction data to threadData, each line corresponding to the next thread
    for (int i = 0; i < NUM_THREADS && fgets(line, sizeof(line), file) != NULL; i++) {
        char* token = strtok(line, " ");
        int txCount = 0;

        // Populate threadData when still below max transactions and a transaction or account number is found
        while (token != NULL && txCount < MAX_TRANSACTIONS - 1) {
            int accountNum = atoi(token);
            token = strtok(NULL, " ");
            if (token == NULL) break; // Malformed input check
            int amount = atoi(token);
            threadData[i].transactions[txCount].accountNum = accountNum;
            threadData[i].transactions[txCount].amount = amount;
            txCount++;
            token = strtok(NULL, " ");
        }
        threadData[i].numTransactions = txCount;
    }

    fclose(file);   // Close the file once all information has been gathered

    // Create and run threads for each set of transactions
    pthread_t threads[NUM_THREADS];
    ThreadParams params[NUM_THREADS];

    // For each thread, populate params with the thread data for transactions to be processed within processTransactions method
    for (int i = 0; i < NUM_THREADS; i++) {
        params[i].threadID = i;
        params[i].data = &threadData[i];

        // If thread is not created correctly, print error
        if (pthread_create(&threads[i], NULL, processTransactions, (void *)&params[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print final balances
    printf("\nFinal Balance\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("    Account %d: $%d\n", i + 1, accounts[i].balance);
    }

    // Cleanup mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
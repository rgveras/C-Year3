#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9                  // Size of each row, column and subgrid
#define THREADS_COUNT 28        // Total numbers of threads, 27 "child" threads, 1 parent

//
// Created by Ricardo Veras on 2024-02-17.
//

int sudoku[SIZE][SIZE];         // 9x9 sudoku initialized
int row_valid[SIZE] = {0};      // Row initialized as invalid
int col_valid[SIZE] = {0};      // Column initialized as invalid
int subgrid_valid[SIZE] = {0};  // Subgrid initialized as invalid

// Check if selected row is valid
void *check_row(void *arg) {
    int row = *((int *)arg);
    int seen[SIZE] = {0};

    // Iterate through numbers in row, checking it's within the bounds allowed in sudoku and if it has been seen before
    for (int j = 0; j < SIZE; j++) {
        int num = sudoku[row][j];

        // If a number falls outside the bounds allowed in sudoku or has been seen in this row, the row is marked invalid, thread exits
        if (num < 1 || num > 9 || seen[num - 1]) {
            row_valid[row] = 0;
            pthread_exit(NULL);
        }
            seen[num - 1] = 1;
    }
    // Row set to valid if all sudoku rules followed
    row_valid[row] = 1;
    // Free memory
    free(arg);
    // Exit thread
    pthread_exit(NULL);
}

// Check if selected column is valid
void *check_column(void *arg) {
    int col = *((int *)arg);
    int seen[SIZE] = {0};

    // Iterate through numbers in column, checking it's within the bounds allowed in sudoku and if it has been seen before
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][col];

        // If a number falls outside the bounds allowed in sudoku or has been seen in this column, the column is marked invalid, thread exits
        if (num < 1 || num > 9 || seen[num - 1]) {
            col_valid[col] = 0;
            pthread_exit(NULL);
        }
            seen[num - 1] = 1;
    }
    // Column set to valid if all sudoku rules followed
    col_valid[col] = 1;
    // Free memory
    free(arg);
    // Exit thread
    pthread_exit(NULL);
}

// Check if selected subgrid is valid
void *check_subgrid(void *arg) {
    int subgrid_num = *((int *)arg);

    int start_row = (subgrid_num / 3) * 3;
    int start_col = (subgrid_num % 3) * 3;
    int seen[SIZE] = {0};

    // Iterate through numbers in subgrid, checking it's within the bounds allowed in sudoku and if it has been seen before
    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num - 1]) {
                subgrid_valid[subgrid_num] = 0;             // If a number falls outside the bounds allowed in sudoku or has been seen in this subgrid, it is marked invalid
                pthread_exit(NULL);                         // Thread exits if subgrid found to be invalid
            }
                seen[num - 1] = 1;
        }
    }
    // Subgrid set to valid if all sudoku rules followed
    subgrid_valid[subgrid_num] = 1;
    // Free memory
    free(arg);
    // Exit thread
    pthread_exit(NULL);
}

// Utilizes check_row, check_column and check_subgrid functions to verify each row, column and subgrid is valid within a sudoku, using a different thread for each
int main(int argc, char *argv[]) {

    // Print error if wrong number of arguments provided
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");

    // If file isn't found, print error message
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Read Sudoku grid from file
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (fscanf(file, "%d", &sudoku[i][j]) != 1) {
                fprintf(stderr, "Error reading from file\n");   // Print error message if file cannot be read
                fclose(file);
                return 1;
            }
        }
    }
    // Close file
    fclose(file);

    // Create thread identifiers
    pthread_t threads[THREADS_COUNT];

    // Create threads to check subgrids
    for (int i = 0; i < SIZE; i++) {
        int *subgrid_ptr = malloc(sizeof(int));
        *subgrid_ptr = i;
        pthread_create(&threads[i + 2*SIZE], NULL, check_subgrid, subgrid_ptr);
    }

    // Create threads to check rows
    for (int i = 0; i < SIZE; i++) {
        int *row_ptr = malloc(sizeof(int));                 // Allocate memory for an integer
        *row_ptr = i;                                            // Assign the loop variable's value to the allocated memory
        pthread_create(&threads[i], NULL, check_row, row_ptr);   // Pass the pointer to the thread function
    }

    // Create threads to check columns
    for (int i = 0; i < SIZE; i++) {
        int *col_ptr = malloc(sizeof(int));
        *col_ptr = i;
        pthread_create(&threads[i + SIZE], NULL, check_column, col_ptr);
    }

    // Join threads
    for (int i = 0; i < THREADS_COUNT-1; i++) {
        pthread_join(threads[i], NULL);
    }

    int valid = 1;      // Initialize validity as being valid
    int thread = 0; // Initialize thread counter to zero

    // Check validity of all subgrids, updating the value of variable "valid" to 0 if invalid, print result
    for (int i = 0; i < SIZE; i++) {
        if (!subgrid_valid[i]) {
            valid = 0;
            thread++;
            printf("Thread # %d (subgrid %d) is INVALID\n", thread, i+1);
        }
        else {
            thread++;
            printf("Thread # %d (subgrid %d) is valid\n", thread, i+1);
        }
    }

    // Check validity of all rows, updating the value of variable "valid" to 0 if invalid, print result
    for (int i = 0; i < SIZE; i++) {
        if (!row_valid[i]) {
            valid = 0;
            thread++;
            printf("Thread # %d (row %d) is INVALID\n", thread, i+1);
        }
        else {
            thread++;
            printf("Thread # %d (row %d) is valid\n", thread, i+1);
        }
    }

    // Check validity of all columns, updating the value of variable "valid" to 0 if invalid, print result
    for (int i = 0; i < SIZE; i++) {
        if (!col_valid[i]) {
            valid = 0;
            thread++;
            printf("Thread # %d (column %d) is INVALID\n", thread, i+1);
        }
        else {
            thread++;
            printf("Thread # %d (column %d) is valid\n", thread, i+1);
        }
    }

    // Print sudoku solution result
    if (valid) {
        printf("%s contains a valid sudoku solution\n", argv[1]);
    } else {
        printf("%s contains an invalid sudoku solution\n", argv[1]);
    }

    return 0;
}

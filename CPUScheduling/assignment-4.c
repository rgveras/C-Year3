//
// Created by Ricardo Veras on 2024-03-12.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

// Process structure
struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int waitTime;
    int turnaroundTime;
};

// Global variables
struct Process processes[MAX_PROCESSES];
int n;
int quantum = 0;

// Function prototypes
void readInput(char *filename);
void runFCFS();
void runSJF();
void runRR();
void printResults();


// This program takes in a file and a parameter to determine which type of algorithm to run, then simulates running processes using the selected algorithm
int main(int argc, char *argv[]) {

    // If not enough arguments are given, display the format that should be used in the command line
    if (argc < 3) {
        printf("Usage: %s [-f | -s | -r] <input_file> <quantum>\n", argv[0]);
        return 1;
    }

    char *algorithm = argv[1];  // The first used argument is algorithm type
    char *filename = argv[2];   // The second used argument is the specified filename

    if (strcmp(algorithm, "-r") == 0) {
        if (argc < 4) {
            printf("Round Robin algorithm requires a quantum parameter.\n");
            return 1;
        } else {
            quantum = atoi(argv[2]);    // If round robin is selected, set second used argument to variable quantum and filename to third used argument
            filename = argv[3];

        }

        // If quantum was set to zero, print error and exit progra2
        if (quantum == 0) {
            perror("Quantum must be greater than 0\n");
            exit(0);
        }
    }

    readInput(filename);


    // Checks arguments and runs selected algorithm
    if (strcmp(algorithm, "-f") == 0) {
        runFCFS();
    } else if (strcmp(algorithm, "-s") == 0) {
        runSJF();
    } else if (strcmp(algorithm, "-r") == 0) {
        runRR();
    } else {
        printf("Invalid algorithm option.\n");      // Prints if an invalid algorithm was inputted
        return 1;
    }

    return 0;
}

// Reads the input of the specified file. Prints an error if the file is not found
void readInput(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    n = 0;  // n is the number of processes in the file
    int arrivalTime = 0; // Initialize arrival time for the first process
    while (fscanf(file, "P%d,%d\n", &processes[n].pid, &processes[n].burstTime) == 2) {
        processes[n].arrivalTime = arrivalTime; // Set arrival time
        processes[n].waitTime = 0;
        processes[n].turnaroundTime = 0;
        n++;
        arrivalTime++;  // Increment arrival time for next process
    }

    fclose(file);
}

// Checks if all processes burst counters are zero, returning 0 if not finished, 1 if finished
int isFinished() {
    int i = 0;
    while (i < n) {
        if (processes[i].burstTime != 0) {
            return 0;
        } i++;
    }

    // Return 1 if all processes are complete
    return 1;
}

// Increments turnaroundTime and waitTime for the necessary processes, decrements burst time if active process
void updateProcesses(int currTime, int pid) {
   int i = 0;
   while (i < n) {
       if (pid == i) processes[i].burstTime--;
       if (currTime >= processes[i].arrivalTime && (processes[i].burstTime != 0)) processes[i].turnaroundTime++;
       if ((currTime >= processes[i].arrivalTime) && (processes[i].burstTime != 0) && (pid != i)) processes[i].waitTime++;
       i++;
   }
}

// Finds the shortest process that has arrived, returns the process number
int findShortest(int currTime) {
    int shortest = 0;
    int i = 0;

    while (i < n) {
        // If a process is finished but the list of processes are not, increment to access the next process, setting it to the currently shortest process
        while (processes[shortest].burstTime <= 0 && i < n) {
            i++;
            if (i == n) break;
            shortest = i;
        }

        // If the processes burst time is less than the currently shortest burst time, we are not at the end of the process list, the process is not finished, and the process has arrived, set it to shortest, increment i
        if (processes[i].burstTime <= processes[shortest].burstTime && i < n && processes[i].burstTime != 0 && processes[i].arrivalTime <= currTime) {
            shortest = processes[i].pid;
            i++;
        } else i++;                 // Increment i to get to the next process if current process is not the shortest

        if (isFinished()) i = n;    // If all processes are finished, set i to n to break out of the loop
    }
    return shortest;                // Return the shortest viable process
}

// Run a process simulation using first come first served
void runFCFS() {
    int currentTime = 0;
    double totalWaitTime = 0;
    double totalTurnaroundTime = 0;

    printf("First Come First Served\n");

    // For each process, print the current time, process number, burst left, wait time, turnaround time, updating the processes values and current time after each print
    for (int i = 0; i < n; i++) {
        while (processes[i].burstTime > 0) {

            printf("T%d : P%d  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", currentTime, processes[i].pid, processes[i].burstTime, processes[i].waitTime, processes[i].turnaroundTime);
            updateProcesses(currentTime, processes[i].pid);
            currentTime++;
        }
    }

    // Calculate the total wait and turnaround time of all processes
    for (int i = 0; i < n; i++) {
        totalWaitTime += processes[i].waitTime;
        totalTurnaroundTime += processes[i].turnaroundTime + 1; // Add 1 to account for the burst time of each process going from 1 to 0
    }

    // Calculate the average wait and turnaround time
    double avgWaitTime = totalWaitTime / n;
    double avgTurnaroundTime = totalTurnaroundTime / n;

    printf("\nTotal average waiting time:     %.2lf\n", avgWaitTime);
    printf("Total average turnaround time:  %.2lf\n", avgTurnaroundTime);
}

// Run a process simulation using shortest job first
void runSJF() {
    int currentTime = 0;
    double totalWaitTime = 0;
    double totalTurnaroundTime = 0;
    int i = 0;

    printf("Shortest Job First\n");

    // While there are still unfinished processes
    while (!isFinished()) {

        // While there are unfinished processes and the current process burst time is greater than 0, print process info, update all necessary processes and current time
        while (processes[i].burstTime > 0 && !isFinished()) {
            printf("T%d : P%d  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", currentTime, processes[i].pid, processes[i].burstTime, processes[i].waitTime, processes[i].turnaroundTime);

            updateProcesses(currentTime, i);
            currentTime++;

            int temp = i;                                               // Set temp to i to switch back if shorter process has not arrived yet
            i = findShortest(currentTime);                     // Find shortest process
            if (processes[i].arrivalTime > currentTime) i = temp;       // If the shorter process has not arrived yet, current process continues
        }
    }

    // Calculate the total wait and turnaround time of all processes
    for (int i = 0; i < n; i++) {
        totalWaitTime += processes[i].waitTime;
        totalTurnaroundTime += processes[i].turnaroundTime + 1; // Add 1 to account for the burst time of each process going from 1 to 0
    }

    // Calculate the average wait and turnaround time
    double avgWaitTime = totalWaitTime / (double)n;
    double avgTurnaroundTime = totalTurnaroundTime / (double)n;

    printf("\nTotal average waiting time:     %.2lf\n", avgWaitTime);
    printf("Total average turnaround time:  %.2lf\n", avgTurnaroundTime);
}

// Run a process simulation using round robin with specified quantum given in arguments
void runRR() {
    int currentTime = 0;
    double totalWaitTime = 0;
    double totalTurnaroundTime = 0;

    printf("Round Robin with Quantum %d\n", quantum);

    // While there are still unfinished processes
    while (!isFinished()) {

        // Iterate through each process
        for (int i = 0; i < n; i++) {
            int remainingQuantum = quantum;     // Reset quantum for each process

            // If the burst time is greater than zero, check if it is less than the remaining quantum or not. If shorter than quantum, run until finished. If quantum is shorter, run until quantum hits 0
            if (processes[i].burstTime > 0) {
                if (processes[i].burstTime < remainingQuantum && processes[i].burstTime != 0) {

                    while (processes[i].burstTime > 0) {

                        // Print current process info, update all necessary process info, increment the current time
                        printf("T%d : P%d  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", currentTime, processes[i].pid, processes[i].burstTime, processes[i].waitTime, processes[i].turnaroundTime);
                        updateProcesses(currentTime, processes[i].pid);
                        currentTime++;
                    }
                } else {
                    // If the remaining quantum is less than the burst time, run the process, updating necessary values, until the quantum is 0
                    while (remainingQuantum > 0) {

                        printf("T%d : P%d  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", currentTime, processes[i].pid, processes[i].burstTime, processes[i].waitTime, processes[i].turnaroundTime);
                        updateProcesses(currentTime, processes[i].pid);
                        remainingQuantum--;
                        currentTime++;
                    }
                }
            }
        }
    }

    // Calculate the total wait and turnaround time of all processes
    for (int i = 0; i < n; i++) {
        totalWaitTime += processes[i].waitTime;
        totalTurnaroundTime += processes[i].turnaroundTime + 1; // Add 1 to account for the burst time of each process going from 1 to 0
    }

    // Calculate the average wait and turnaround time
    double avgWaitTime = totalWaitTime / n;
    double avgTurnaroundTime = totalTurnaroundTime / n;

    printf("\nTotal average waiting time:     %.2lf\n", avgWaitTime);
    printf("Total average turnaround time:  %.2lf\n", avgTurnaroundTime);
}

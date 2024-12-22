#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// This function determines if a number is a prime number, returning 1 if it is, 0 otherwise
int isPrime(long int number) {

    long int i;
    for(i=2; i<number; i++) {
        long int primeCheck = number % i;
        if (primeCheck % i == 0 && i != number) {
            return 0;
        }
    }
    return 1;
}

// Check if each number between start and end is a prime number, returning 1 if it is a prime and 0 if it is not
int prime(long int start, long int end) {

        // Count keeps track of the current number to be checked, primes counts the number of prime numbers found
        long int count;
        long int primes = 0;

        // If the start is 0, convert start to 2 as 0,1 and numbers divisible by 1 cannot be considered a prime number
        if (start == 0) {
            count = 2;
        } else count = start;

        // Start the sum of the prime numbers at 0
        long int sum =0;

        // Check if each number is a prime number using the isPrime() function. Increment count. Run until count reaches end
        do {
            int check = isPrime(count);

            if(check == 1) {
                primes++;
                sum += count;
            } else  {

            }
            count++;
        } while(count < end);

        printf("PID: %d, PPID: %d, Sum between %ld and %ld is %ld. Count is %ld\n", getpid(), getppid(), start, end, sum, primes);

        return 0;
    }

// Create 4 equal intervals and check all numbers in each interval to determine if they are prime
int main(int argc, char * argv[]) {

    // Parse command line arguments
    int arg1 = atoi(argv[1]);
    long int arg2 = atoi(argv[2]);
    long int arg3 = atoi(argv[3]);

    // Split total interval into 4 equal-sized intervals
    long int size = (arg3 - arg2) / 4;

    // Intervals created to split the total number into four intervals before calculating prime numbers
    long int start1 = arg2;
    long int end1 = arg2 + size;
    long int start2 = arg2 + size + 1;
    long int end2 = arg2 + size * 2;
    long int start3 = arg2 + (size * 2) + 1;
    long int end3 = arg2 + size * 3;
    long int start4 = arg2 + (size * 3) + 1;
    long int end4 = arg3;

    // Prime numbers sum and count; split into four processes in series as specified by arg1 == 0.
    if (arg1 == 0) {
        prime(start1, end1);
        prime(start2, end2);
        prime(start3, end3);
        prime(start4, end4);

    } else if (arg1 == 1) {
        // Initializing and creating child processes via fork()
        pid_t child1, child2;
        child1 = fork();
        child2 = fork();

        // If there is an error in the forking process, print an error
        if (child1 == -1 || child2 == -1) {
            perror("Error in fork");
            exit(EXIT_FAILURE);
        }

        // Run processes in parallel, with the first interval running first, followed by the second, third and fourth interval
        if (child2 == 0 && child1 == 0) {

            prime(start4, end4);
        } else if (child2 == 0) {

            prime(start2, end2);
        } else if (child1 != 0) {

            prime(start1, end1);
        }

        if (child1 == 0 && child2 != 0) {
            prime(start3, end3);
        }
    }
}

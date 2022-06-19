// Guy Adani 208642884

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define TIMEOUT_TIME 60
#define BUFF_SIZE 200

// In case an error occurred
void timeToDie() {
    remove("to_srv.txt");
    printf("ERROR_FROM_EX48\n");
    exit(1);
}

// Calculate the result to write
int calculate(int a, int action, int b) {
    int result = 0;
    switch (action) {
        case 1:
            result = a + b;
            break;
        case 2:
            result = a - b;
            break;
        case 3:
            result = a * b;
            break;
        case 4:
            result = a / b;
            break;
        default:
            timeToDie();
    }
    return result;
}

// In case there was a timeout
void timeoutHandler() {
    while(wait(NULL) > 0);
    printf("The server was closed because no service request was received for the last %d seconds\n", TIMEOUT_TIME);
    exit(1);
}

// Handle the clients
void handleClient() {
    alarm(0);   // Set off the alarm

    // Forking to handle the client
    int pid;
    if ((pid = fork()) < 0) timeToDie();
    if (pid == 0) {     // Child
        // Initialize
        int args[4], result, fd; // usage: file input, calculate, file descriptor
        char buffer[BUFF_SIZE];

        // Open the to_srv file
        FILE *fs = fopen("to_srv.txt", "r");
        if (fs == NULL) timeToDie();

        // Read the file
        int i;
        for(i = 0; i < 4; i++) {
            if(fgets(buffer, BUFF_SIZE, fs) == NULL) timeToDie();
            args[i] = atoi(buffer);
        }

        // Immediately close and delete file for next client
        if(fclose(fs)) timeToDie();
        remove("to_srv.txt");

        // Calculate result and preparing to write
        char string[BUFF_SIZE];
        if (args[2] == 4 && args[3] == 0) {     // Divide by 0!
            sprintf(string, "CANNOT_DIVIDE_BY_ZERO\n");
        } else {
            result = calculate(args[1], args[2], args[3]);
            sprintf(string, "%d\n", result);
        }


        // Write to client
        char fileName[BUFF_SIZE];
        sprintf(fileName, "to_client_%d.txt", args[0]);
        fs = fopen(fileName, "w");
        if (fs == NULL) timeToDie();
        if (fputs(string, fs) < 0) timeToDie();
        if (fclose(fs) < 0) timeToDie();

        // Notify client and delete
        kill(args[0], SIGUSR1);
        exit(0);

    }

    // Parent
    alarm(TIMEOUT_TIME);
//    pause();
}

int main() {

    // Set handler for clients
    sigset_t  block_mask;
    sigfillset(&block_mask);
    struct sigaction usr_action;
    usr_action.sa_handler = handleClient;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;

    sigaction(SIGUSR2, &usr_action, NULL);

    // Set timeout signal and handler
    usr_action.sa_handler = timeoutHandler;
    sigaction(SIGALRM, &usr_action, NULL);
    alarm(TIMEOUT_TIME);
    while(1) pause();
}

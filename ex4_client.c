// Guy Adani 208642884

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// TODO change to 30
#define TIMEOUT_TIME 5             // Timeout when reaching the server
#define RANDOM 1 + (rand() % 6)     // Random seconds between tries

// Exit because of an error
void timeToDie() {
    remove("to_srv.txt");
    printf("ERROR_FROM_EX4\n");
    exit(1);
}

// Writing to the server
void writeToFile(char *argv[]) {
    int fd, i;
    for(i = 0; i < 10; i++) {
        // Trying to open the file
        if((fd = open("to_srv.txt", O_WRONLY | O_CREAT | O_EXCL)) < 0) {
            sleep(RANDOM);
            continue;
        }

        // Preparing the text
        char string[100];
        sprintf(string, "%d", (int)getpid());
        for(i = 2; i < 5; i++) {

            strcat(string, "\n");
            strcat(string, argv[i]);
        }

        // Writing and closing
        if (write(fd, string, strlen(string)) < 0) timeToDie();
        if (close(fd) < 0) timeToDie();
        return;
    }

    timeToDie();
}

// Handling the server answer
void serverAnswer() {
    alarm(0);
    exit(0);
}

// Handling the timeout
void timeoutHandler() {
    printf("Client closed because no response was received from the server for %d seconds\n", TIMEOUT_TIME);
    remove("to_srv.txt");
    exit(1);
}


int main(int argc, char *argv[]) {

    // Checking arguments
    if (argc > 5) timeToDie();

    // Checking valid action
    if (atoi(argv[3]) < 1 || atoi(argv[3]) > 4) timeToDie();

    writeToFile(argv);

    signal(SIGUSR1, serverAnswer);  // Set signal handler
    kill(atoi(argv[1]), SIGUSR2);   // Send signal to the server
    signal(SIGALRM, timeoutHandler);    // Set timeout handler
    alarm(TIMEOUT_TIME);    // Set timeout
    sleep(TIMEOUT_TIME + 1);
    timeToDie();
}
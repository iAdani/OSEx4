// Guy Adani 208642884

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define TIMEOUT_TIME 60
#define BUFF_SIZE 200

void timeToDie() {
    remove("to_srv.txt");
    printf("ERROR_FROM_EX4\n");
    exit(1);
}

void timeoutHandler() {
    printf("The server was closed because no service request was received for the last %d seconds\n", TIMEOUT_TIME);
    exit(1);
}

void handleClient() {
    // Initialize
    alarm(0);   // Set off the alarm
    int args[4], i, charsRead, pid; // usage: file input, loops, num of chars in buffer, fork
    FILE* fd;
    char buffer[BUFF_SIZE];

    // Open the to_srv file
    if ((fd = fopen("to_srv.txt", "r") < 0)) timeToDie();

    // Read the file
    while(i < 4) {
        if ((charsRead = fgets(buffer, BUFF_SIZE, fd)) < 1) timeToDie();
        buffer[charsRead - 1] = '\0';
        args[i] = atoi(buffer);
        i++;
    }

    // Immediately close and delete file for next client
    close(fd);
    remove("to_srv.txt");

    //Forking to handle the client
    if ((pid = fork()) < 0) timeToDie();
    if (pid == 0) {     // child

    } else {            // parent
        // Set timeout and this function to the next clients
        signal(SIGUSR2, handleClient);
        alarm(TIMEOUT_TIME);
        sleep(TIMEOUT_TIME + 1);
    }



}

int main() {
    //TODO delete it
    printf("Server pid: %d", getpid());

    // Set handler for clients
    signal(SIGUSR2, handleClient);

    // Set timeout signal and handler
    signal(SIGALRM, timeoutHandler);
    alarm(TIMEOUT_TIME);
    sleep(TIMEOUT_TIME +1);
    timeToDie();
}

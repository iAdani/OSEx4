// Guy Adani 208642884

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define TIMEOUT_TIME 30             // Timeout when reaching the server
#define RANDOM 1 + (rand() % 6)     // Random seconds between tries
#define BUFF_SIZE 200

// In case an error occurred
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
        if((fd = open("to_srv.txt", O_WRONLY | O_CREAT | O_EXCL, 0666)) < 0) {
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

    // Making the file name
    char fileName[BUFF_SIZE];
    sprintf(fileName, "to_client_%d.txt", (int)getpid());

    // Open the to_client file
    FILE *fs = fopen(fileName, "r");
    if (fs == NULL) timeToDie();

    // Read file and show result
    char string[BUFF_SIZE];
    fgets(string, BUFF_SIZE, fs);
    printf("%s", string);

    if(fclose(fs) < 0) timeToDie();
    remove(fileName);
    exit(0);
}

// In case there was a timeout
void timeoutHandler() {
    printf("Client closed because no response was received from the server for %d seconds\n", TIMEOUT_TIME);
    remove("to_srv.txt");
    exit(1);
}

// Returns 1 if is a number, otherwise 0
int isANumber(char* string) {
    int i = strlen(string);
    while(i--) {
        if (string[i] > 47 && string[i] < 58) continue;
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {

    // Checking arguments
    if (argc != 5) timeToDie();
    int i;
    for(i = 1; i < 5; i ++) {
        if (!isANumber(argv[i])) timeToDie();
    }

    // Checking valid action
    if (atoi(argv[3]) < 1 || atoi(argv[3]) > 4) timeToDie();

    writeToFile(argv);

    // Set handler for clients
    sigset_t  block_mask;
    sigfillset(&block_mask);
    struct sigaction usr_action;
    usr_action.sa_handler = serverAnswer;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;

    sigaction(SIGUSR1, &usr_action, NULL);

    // Set timeout signal and handler
    usr_action.sa_handler = timeoutHandler;
    sigaction(SIGALRM, &usr_action, NULL);
    alarm(TIMEOUT_TIME);
    kill(atoi(argv[1]), SIGUSR2);   // Send signal to the server
    pause();
    timeToDie();
}

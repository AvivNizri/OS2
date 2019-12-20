#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define ioFile "to_srv"

int main(int argc, char* argv[]){
	if(argc != 5){
		printf("The system got more items than needed..\n exiting...\n");
		return 0;
	}

    // validate client not trying to divide by zero
    if(atoi(argv[3]) == 4 && atoi(argv[4]) == 0){
        printf("Wrong Operation detected - dividation by zero \n");
	    exit(0);
    }

    // lets try to open to_srv file
    int itr = 0;
    srand(time(NULL));
    while(access(ioFile, F_OK) == 0 && itr < 10){
        printf("sleep num %d\n",itr);
        sleep(rand() % 5);
        itr++;
    }

    if(itr==10){
        printf("Warning: Can't create to_srv file, already exist\nExiting...\n");
        exit(0);
	}

    // here that means that now the file isn't exist
    int toClientFile = open(ioFile, O_WRONLY, S_IRWXU);
    char data[1024];
    sprintf(data,"%d %d %d %d", getpid(), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    strcat(data, "\0");
    printf("Data is : %s\n", data);
    write(toClientFile, data, strlen(data));
    close(toClientFile);


}
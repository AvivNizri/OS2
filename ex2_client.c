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
#define resFile "to_client_"

void SIGUSR1_handler(int sig) {
    char buf[512];

    signal(SIGUSR1,SIGUSR1_handler);

    printf("Client pid=%d receives a signal from server %d\n",getpid(),sig); 
    sleep(1);

    // build dest file name
    char id[128];
    sprintf(id,"%d",getpid());
    char* resultFileName = malloc(strlen(resFile) + strlen(id) + 1);
    strcpy(resultFileName, resFile);
    strcat(resultFileName, id);

    int toClientFile = open(resultFileName, O_WRONLY |O_CREAT|O_TRUNC, S_IRWXU);

    if(toClientFile < 0) exit(0);

    if(read(toClientFile, buf, strlen(buf)) > 0){
        printf("Recived calculation from server : %s\n", buf);
    }
    close(toClientFile);
    
}

int main(int argc, char* argv[]){
	if(argc != 5){
		printf("The system got more items than needed..\n exiting...\n");
		return 0;
	}

    // validate client not trying to divide by zero
    if(atoi(argv[3]) == 4 && atoi(argv[4]) == 0){
        printf("Wrong Operation detected - dividation by zero\n");
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

    if(itr == 10){
        printf("Warning: Can't create to_srv file, already exist\nExiting...\n");
        exit(0);
	}
    // that will handle signal from the server
    signal(SIGUSR1,SIGUSR1_handler); 

    // here that means that now the file isn't exist
    int toClientFile = open(ioFile, O_WRONLY |O_CREAT|O_TRUNC, S_IRWXU);
    if(toClientFile < 0){
        printf("Failed opening file\n");
        exit(0);
    }
    char data[1024];
    sprintf(data,"%d %d %d %d", getpid(), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    strcat(data, "\0");
    printf("Data client sent is : %s\n", data);
    write(toClientFile, data, strlen(data));
    close(toClientFile);

    // signal the server to start doing his job
    kill(atoi(argv[1]),SIGUSR1);
    // going idle untill we get signal back
    pause();
}
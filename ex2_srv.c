#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define ioFile "to_srv"
#define resFile "to_client_"
#define SIZE 1024
#define DATA_SIZE 4
#define ID_SIZE 128

void SIGUSR1_handler(int sig){
    // when signal arrived main server, main process will fork here
    printf("********** Server handle new client **********\n");
    
    //signal(SIGUSR1,SIGUSR1_handler);

    pid_t cpid = fork();
    if(cpid == 0){
        // prob[0] = remote process ID | prob[1] = left number
        // prob[2] = math action       | prob[3] = right number
        int prob[DATA_SIZE];
        char buf[SIZE];

        int fromClient = open(ioFile, O_RDONLY);
        if(fromClient <= 0){
            printf("Client file Opening failure\n");
            exit(EXIT_FAILURE);
        }
        printf("client File open\n");
        // read entire file
        int val;
        if(val = read(fromClient, buf, SIZE) <= 0){
            close(fromClient);
            printf("Error reading client file\n");
            exit(EXIT_FAILURE);
        }
        //sleep(1);
        close(fromClient);
        remove(ioFile);

        printf("Got from client : %s\n", buf);

        // parse the data from the file
        char* tokptr = strtok(buf," ");
        for(int i=0 ;i<DATA_SIZE; i++){
            prob[i] = atoi(tokptr);
            tokptr = strtok(NULL," ");
            //printf("Number %d\n", prob[i]);
        }

        // lets start the calculations
        int calc;
        switch (prob[2]){
        case 1:
            calc = prob[1] + prob[3];
            break;
        case 2:
            calc = prob[1] - prob[3];
            break;
        case 3:
            calc = prob[1] * prob[3];
            break;
        case 4:
            calc = prob[1] / prob[3];
            break; 
        }
        printf("Answer to client is %d\n", calc);
        
        // build dest file name
        char id[ID_SIZE];
        sprintf(id,"%d",prob[0]);
        char* resultFileName = malloc(strlen(resFile) + strlen(id) + 1);
        strcpy(resultFileName, resFile);
        strcat(resultFileName, id);

        int toClientFile = open(resultFileName, O_WRONLY |O_CREAT|O_TRUNC, S_IRWXU);
        if(toClientFile < 0) exit(EXIT_FAILURE);
        char res[ID_SIZE];
        sprintf(res,"%d",calc);
        write(toClientFile, res, strlen(res));
        close(toClientFile);

        // return signal to the relevant client
        kill(prob[0],SIGUSR1);
    }
}

int main(int argc, char* argv[]){
    // delete to_srv if exists
    remove(ioFile);
    
    printf("Server pid : %d\n",getpid());
    while(1){
        signal(SIGUSR1,SIGUSR1_handler);
        // now we will wait until signal arrive
    	pause();
    }
}
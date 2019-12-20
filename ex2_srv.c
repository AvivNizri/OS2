#include <stdio.h>
#include <sys/fcntl.h>

#define ioFile "to_srv"

int main(int argc, char* argv[]){
    // delete to_srv if exists
    remove(ioFile);

}
// Grzegorz Rozak cw02 zad2

#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <memory.h>

#define max_arguments 32
#define max_line 256


int main(int argc, char *argv[]) {
  if(argc !=2) {
    printf("Run with filename:\n\t./main file\n");
    exit(1);
  }
  FILE *file=fopen(argv[1], "r"); //readonly
  if(file==NULL) {
    printf("Can not open file\n");
    exit(1);
  }

  char line[max_line];
  char* args[max_arguments];

  int i=0;
  while(fgets(line, max_line, file)!=NULL) {
    printf("\n");
    i=0;
    while((args[i]=strtok(i==0 ? line : NULL , " \n"))!=NULL) {
      i++;
      if(i>max_arguments) {
        printf("Error. Too many arguments in %s\n", args[0]);
      }
    }
    pid_t pid=fork();
    if(pid==0) {
      if(strcmp(args[0],"\0")==0) {
        printf("Empty line in file\n");
        exit(1);
      }
      else {
        execvp(args[0],args);
        printf("Error while executing: %s\n", args[0]); // jeśli zostanie podana błędna komenda
        break;                                          // żeby kolejne komendy nie wykonywały się w dwóch procesach
      }
    }

    int status;
    wait(&status);
    if(status) {
      printf("Error while executing: %s\n", args[0]);
    }
  }


  fclose(file);
  exit(0);
}

// Grzegorz Rozak cw02 zad2

#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <memory.h>
#include <sys/resource.h>

#define max_arguments 32
#define max_line 256

void calculateTimeDiff(struct timeval tv2, struct timeval tv1) {
	long secs= tv2.tv_sec - tv1.tv_sec;
	long msecs = tv2.tv_usec - tv1.tv_usec;
	if(msecs<0) {
		msecs+=1000000;
		secs -=1;
	}
	printf(" time spent: %ld.%06ld", secs, msecs);
}

void setLimits(char* time_l, char* memory_l){
  unsigned long time_limit=strtol(time_l, NULL, 10);
  unsigned long memory_limit=strtol(memory_l, NULL, 10);

  struct rlimit r_cpu;
  struct rlimit r_memory;

  r_cpu.rlim_max=(rlim_t)time_limit;
  r_cpu.rlim_cur=(rlim_t)time_limit * 3/4;
  r_memory.rlim_max=(rlim_t)memory_limit * 1024 * 1024;
  r_memory.rlim_cur=(rlim_t)memory_limit * 1024 * 1024 *3/4;

  if(setrlimit(RLIMIT_CPU, &r_cpu)!=0) {
    printf("Cannot set cpu limit\n");
  }
  if(setrlimit(RLIMIT_DATA, &r_memory)!=0) {
    printf("Cannot set memory limit\n");
  }
}

int main(int argc, char *argv[]) {
  if(argc !=4) {
    printf("Run with filename and limits:\n\t./main file cpu_limit mem_limit\n");
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

  struct rusage time_b;
  struct rusage time_e;

  while(fgets(line, max_line, file)!=NULL) {
    getrusage(RUSAGE_CHILDREN, &time_b);
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

        printf("Executing: ");
        for(int j=0;j<i;j++)
          printf("%s ", args[j]);
        printf(" on pid: %d parent pid: %d\n", getpid(), getppid());
        setLimits(argv[2], argv[3]);
        execvp(args[0],args);
        printf("Error while executingx: %s\n", args[0]); // jeśli zostanie podana błędna komenda
        break;                                          // żeby kolejne komendy nie wykonywały się w dwóch procesach
      }
    }
    int status;
    wait(&status);
    if(status) {
      printf("Error while executing: %s\n", args[0]);
    }
    getrusage(RUSAGE_CHILDREN, &time_e);
    printf("\nUser time:\t");
    calculateTimeDiff(time_e.ru_utime, time_b.ru_utime);
    printf("\nSystem time:\t");
    calculateTimeDiff(time_e.ru_stime, time_b.ru_stime);
    printf("\n\n");
  }


  fclose(file);
  exit(0);
}

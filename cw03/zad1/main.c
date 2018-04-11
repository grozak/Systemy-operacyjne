#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <wait.h>

void printPermissions (char* file_path) {
	struct stat fileStat;
	if(stat(file_path, &fileStat) < 0)
		return;

	printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
  printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
  printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
	printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
  printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
  printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
  printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
	printf("\t");
}

void printAbsolutePath(char* file_path) {
	printf("%s\t", realpath(file_path, NULL));
}

void printSize(char* file_path) {
	struct stat fileStat;
	if(stat(file_path, &fileStat) < 0)
		return;
	printf("%lu bytes\t", fileStat.st_size);
}

void printModTime(char* file_path) {
	struct stat fileStat;
	if(stat(file_path, &fileStat) < 0)
		return;
	char str[32];
	strftime(str, sizeof(str), "%c", localtime(&fileStat.st_mtime));
	printf("%s\t", str);
}

void printAll(char* path, int search_mode, long time) {
	struct stat st;
	stat(path, &st);


	if(search_mode==-1 && st.st_mtime<time) {
		printPermissions(path);
		printSize(path);
		printModTime(path);
		printAbsolutePath(path);
		printf("\n");
	}
	else if(search_mode==0) {
		time_t t=st.st_mtime;
		struct tm tmp;
		localtime_r(&t, &tmp);
		tmp.tm_isdst=tmp.tm_sec=tmp.tm_min=tmp.tm_hour=0;

		if(mktime(&tmp)==time) {
			printPermissions(path);
			printSize(path);
			printModTime(path);
			printAbsolutePath(path);
			printf("\n");
		}

	}
	else if(search_mode==1 && st.st_mtime>time) {
		printPermissions(path);
		printSize(path);
		printModTime(path);
		printAbsolutePath(path);
		printf("\n");
	}
	else
		return;


}

void searchFiles(char* dir_path, int search_mode, long time) {
	pid_t pid;

	DIR *dir = opendir(dir_path);
	if(dir==NULL) {
		printf("Wrong path\n");
		exit(1);
	}

	struct dirent* file;
	while((file=readdir(dir))!=NULL) {
		if(strcmp(file->d_name, ".") && strcmp(file->d_name, "..")) {
			if(file->d_type==DT_REG) {
				char *temp_path = calloc(1000, sizeof(char));
				strcpy(temp_path, realpath(dir_path, NULL));
				strcat(temp_path, "/");
				strcat(temp_path, file->d_name);
				printAll(temp_path, search_mode, time);
				free(temp_path);
			}
			else if(file->d_type==DT_DIR) {
				char *temp_path = calloc(1000, sizeof(char));
				strcpy(temp_path, realpath(dir_path, NULL));
				strcat(temp_path, "/");
				strcat(temp_path, file->d_name);


				pid=fork();			//nowy proces
				if(pid==0) {
					searchFiles(temp_path, search_mode, time);
					exit(0);
				}

			}
		}
		int status;
		wait(&status);
	}
	closedir(dir);
}


int main(int argc, char *argv[]){
	if(argc<4) {
		printf("Too few arguments. Use:\n\t\t ./main file_path operand(<, =, >) date(ddmmyy)\n");
		return 1;
	}

	int mode;
	if(argv[2][0]=='<')
		mode=-1;
	else if(argv[2][0]=='=')
		mode=0;
	else if(argv[2][0]=='>')
		mode=1;
	else {
		printf("Wrong operand \n");
		return 1;
	}

	if(strlen(argv[3])!=8) {
		printf("wrong date\n");
		return 1;
	}
	char year[5]={argv[3][0], argv[3][1], argv[3][2], argv[3][3], '\0'};
	char month[3]={argv[3][4], argv[3][5], '\0'};
	char day[3]={argv[3][6], argv[3][7], '\0'};


	struct tm tmp;
	tmp.tm_year=(int) (strtol(year, NULL, 10)-1900);
	tmp.tm_mon=(int) (strtol(month, NULL, 10)-1);
	tmp.tm_mday=(int) strtol(day, NULL, 10);
	tmp.tm_sec=tmp.tm_min=tmp.tm_hour=tmp.tm_isdst=0;

	printf("I start\n");
	searchFiles(argv[1], mode, mktime(&tmp));
	printf("I finished\n");

	exit(0);
}

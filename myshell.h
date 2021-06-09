/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : MyShell phase3								 *
 * FIle name    : myshell.h     								 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 05 - 30								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define MAXARGS 128 
#define MAXLINE 8192
#define MAXFILE 256
#define MAXJOBS 100

typedef struct _jobStruct{
	int status; /* Process status [0: Stopped] [1: Sleeping] [2: Running] */
	int bg; /* Background : 1 */
	int pid; /* Process ID */
	char cmd[100]; /* Process command */
}jobStruct;

jobStruct bg_job[MAXJOBS];
static int bg_jobNum;

int fg_pid;
char fg_cmd[100];

sigset_t mask;

/* function */
void handler(int);
int getStatus(int);
int getJobIdx(int);
void deleteJob(int, char);
void addJob(int, char*);
void listJobs();
int updateJob(int, int);
int CMD_fgbgkill(char** argv, int bg);
void check_jobs();
void eval(char *);
int builtin_command(char**);
int parseline(char*, char**);
int parsepipe(char*, char*, char **);
void makepipe(char**, char*);


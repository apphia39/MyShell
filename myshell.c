/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : MyShell phase3								 *
 * FIle name    : myshell.c     								 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 05 - 30								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "myshell.h"

void handler(int signo){
	/* signal must kill only foreground processes */

	if(fg_pid){
		switch(signo){
			case SIGINT: 
						 fprintf(stdout, "\n");
						 kill(fg_pid, SIGINT);
						 deleteJob(fg_pid, 'T');
						 break;
			case SIGTSTP: 
						  fprintf(stdout, "\n");
						  kill(fg_pid, SIGTSTP);
						  addJob(fg_pid, fg_cmd);
						  break;
			case SIGCHLD:
						  while(waitpid(-1, NULL, WNOHANG) > 0);
						  break;
		}
	}
}

/* Return the bg_job's index of current pid */
int getJobIdx(int pid){
	for(int i=0; i<bg_jobNum; i++){
		if(bg_job[i].pid == pid)
			return i;
	}
	return -1;
}

/* Return the status of current pid */
int getStatus(int pid){
	char filePath[MAXFILE];
	char line[MAXFILE];
	char status[10];
	FILE *fp;
	int tmp_int;
	char tmp_state[20];

	if(kill(pid, 0) == -1){
		fprintf(stdout, "*** PID[%d] does not exists ***\n", pid);
		return -1;
	}

	/* Get status from /proc/[pid]/stat */
	sprintf(filePath, "/proc/%d/stat", pid);
	fp = fopen(filePath, "r");
	fgets(line, sizeof(line), fp);
	sscanf(line, "%d %s %s\n", &tmp_int, tmp_state, status);
	fclose(fp);

	if(strcmp(status, "T") == 0) /* Stopped */
		return 0;
	else if(strcmp(status, "S") == 0) /* Sleeping */
		return 1;
	else if(strcmp(status, "R") == 0) /* Running */
		return 2;
	else if(strcmp(status, "Z") == 0) /* Zombie */
		return 3;

	return -1;
}

/* Delete current pid from bg_job */
void deleteJob(int pid, char type){
	int idx = getJobIdx(pid);
	if(idx == -1){
		return ;
	}
	
	if(type == 'K') /* current pid is killed by 'kill' command */
		fprintf(stdout, "[%d] Killed\t\t%s\n", idx+1, bg_job[idx].cmd);
	else /* current pid is terminated by 'ctrl_c' signal */
		fprintf(stdout, "[%d] Terminated\t\t%s\n", idx+1, bg_job[idx].cmd);

	/* initialize current pid's bg_job */
	bg_job[idx].pid = -1;
	bg_job[idx].bg = 0;
	memset(bg_job[idx].cmd, 0, sizeof(bg_job[idx].cmd));

	if(idx == bg_jobNum - 1)
		bg_jobNum--;
}

/* Add current pid into bg_job */
void addJob(int pid, char *cmdline){
	int status;
	int idx;
	
	status = getStatus(pid); /* Get status of current process*/
	idx = getJobIdx(pid);	 /* Get job index of current process */
	if(idx == -1){ /* current pid is already in the job */
		idx = bg_jobNum;
		bg_jobNum++;
	}

	/* Get rid of & */
	for(int i=strlen(cmdline) - 1; i>=0; i--){
		if(cmdline[i] == '&' || cmdline[i] == ' ' || cmdline[i] == '\n')
			cmdline[i] = '\0';
		else
			break;
	}

	/* Add into JOB */
	bg_job[idx].status = status;

	bg_job[idx].pid = pid;
	bg_job[idx].bg = 1;
	strcpy(bg_job[idx].cmd, cmdline);

	if(status == 0) /* If process is stopped */
		fprintf(stdout, "[%d]+ Stopped \t\t%s\n", idx+1, cmdline);
	else /* If process is running on the background */
		fprintf(stdout, "[%d] %d\n", idx+1, pid);

	return;
}

/* Print all jobs in the bg_job */
void listJobs(){
	for(int idx = 0; idx < bg_jobNum; idx++){ /* Read all bg_job */
		if(bg_job[idx].pid != -1){ /* Print only valid bg_job */
			switch(bg_job[idx].status){
				case 0: fprintf(stdout, "[%d] Stopped\t\t%s\n", idx+1, bg_job[idx].cmd);
						break;
				case 1: 
				case 2: fprintf(stdout, "[%d] Running\t\t%s &\n", idx+1, bg_job[idx].cmd);
						break;
			}
		}
	}
}

int updateJob(int pid, int bg){
	int idx = getJobIdx(pid); /* Get the bg_job's index of current pid */

	if (idx == -1 || bg_job[idx].pid == -1) /* Invalid index */
		return -1;

	/* Update the status of current pid */
	bg_job[idx].status = getStatus(pid);

	if(bg == 0){ /* fg <job> */
		if(bg_job[idx].status == 0 || bg_job[idx].status == 1){ /* Only for Running and Stopped background Jobs */
			fprintf(stdout, "%s\n", bg_job[idx].cmd);
			
			bg_job[idx].status = 2;
			bg_job[idx].bg = 0;
			
			kill(pid, SIGCONT);

			/* Update fg */
			fg_pid = pid;
			strcpy(fg_cmd, bg_job[idx].cmd);
			waitpid(pid, NULL, WUNTRACED);
		
			return 0;
		}
	}
	else if(bg == 1){ /* bg <job> */
		if(bg_job[idx].status == 0){ /* Only for Stopped background Jobs */
			bg_job[idx].bg = 1;
			bg_job[idx].status = 1;

			kill(pid, SIGCONT);
			return 0;
		}
	}
	else{ /* kill <job> */
		kill(pid, SIGKILL);
		deleteJob(pid, 'K');
		return 0;
	}
	return -1;
}

int CMD_fgbgkill(char **argv, int bg){
	/* bg [0: fg] [1: bg] [-1: kill] */
	int idx;
	int pid;

	if(argv[1][0] == '%'){ /* Get JobID */
		idx = atoi(argv[1] + 1) - 1;
		if(idx < 0 || idx >= bg_jobNum) /* Check if JobID is valid */
			return -1;
		pid = bg_job[idx].pid; /* Get PID */
	}
	else{ /* Get PID */
		pid = atoi(argv[1]);
	}	
	return updateJob(pid, bg);
}

/* check if there is completed process in bg_job */
void check_jobs(){
	char filePath[MAXFILE];		/* filePath */
	char line[MAXFILE];			/* Read line from filePath */
	int status;					/* Process Status */
	char cmd[100];				/* Process cmdline */
	pid_t pid;					/* Process ID */
	char c;	
	FILE* fp;

	if(bg_jobNum == 0) /* bg_job is empty */
		return;

	/* Find terminated JOB */
	for(int idx = bg_jobNum-1; idx >= 0; idx--){
		pid = bg_job[idx].pid;

		if(pid == -1) /* Do not check empty job */
			continue;

		status = getStatus(pid);
		bg_job[idx].status = status;

		if(status == 0 || status == 1 || status == 2){
			bg_job[idx].status = status;
			continue;
		}
		else{
			fprintf(stdout, "\n[%d] Done\t\t %s\n", idx+1, bg_job[idx].cmd);

			/* initialize bg_job[idx] */
			bg_job[idx].pid = -1;
			memset(bg_job[idx].cmd, 0, sizeof(bg_job[idx].cmd));
			
			/* If idx is last index of bg_job, decrease bg_jobNum */
			if(idx == bg_jobNum-1){
				bg_jobNum--;
			}
		}
	}
}

int main(){
	char cmdline[MAXLINE]; /* Command line */

	/* Initialize */
	bg_jobNum = 0;
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCHLD, handler);

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	while(1){
		fg_pid = 0;
		check_jobs();

		/* Prompt */
		printf("CSE4100-SP-P4> ");

		/* Read */
		fgets(cmdline, MAXLINE, stdin);
		if(feof(stdin))
			_exit(0);

		/* Evaluate */
		eval(cmdline);
	}
}

void eval(char *cmdline){
	char *argv[MAXARGS];	/* Argument list execvp() */
	char buf[MAXLINE];		/* Holds modified command line */
	int bg;					/* Should the job run in bg or fg? */
	pid_t pid;				/* Process id */
	int child_status;		/* Child status */
	int pipe = 0;			/* Pipe or Nonpipe? */
	char tmpBUF[MAXLINE];
	char tmpCMD[MAXLINE];
	
	strcpy(buf, cmdline);
	bg = parseline(buf, argv);		/* [1: background job] [0: foreground job] */
	if(argv[0] == NULL) return ;	/* Ignore empty lines */

	/* Check if command includes pipe */
	if(strchr(cmdline, '|') != NULL){
		pipe = 1;
		strcpy(tmpBUF, cmdline);
	}

	/* Execute */
	if(!builtin_command(argv)){
		pid = fork();
	
		if(pid < 0){/* fork error */
			fprintf(stderr, "*** Fork error ***\n");
			_exit(0);
		}
		else if(pid == 0){/* child process */
			if(bg)
				sigprocmask(SIG_BLOCK, &mask, NULL);
			else
				sigprocmask(SIG_UNBLOCK, &mask, NULL);

			if(pipe == 1){ /* Pipe */
				parsepipe(tmpBUF, tmpCMD, argv); /* Get the command of first pipe */
				makepipe(argv, tmpCMD);
				_exit(0);
			}
			else{/* No Pipe */
				if(execvp(argv[0], argv) < 0){
					fprintf(stderr, "%s: Command not found.\n", argv[0]);
					_exit(0);
				}
			}
		}
		else{/* parent process */
			if(bg){
				sigprocmask(SIG_BLOCK, &mask, NULL);
				addJob(pid, cmdline);
			}
			
			sigprocmask(SIG_UNBLOCK, &mask, NULL);

			if(!bg){ /* Foreground */
				memset(fg_cmd, 0, sizeof(fg_cmd));
				fg_pid = pid;
				strcpy(fg_cmd, cmdline);

				if(waitpid(pid, &child_status, WUNTRACED) < 0){
					fprintf(stderr, "*** Wait error ***\n");
					_exit(0);
				}
			}
			else{ /* Background */
				return ;
			}
		}
	}

	return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv){

	if(strcmp(argv[0], "cd") == 0){		/* cd command */
		if(argv[1]){
			if(chdir(argv[1]) < 0){
				fprintf(stderr, "-bash: %s: %s: No such file or directory\n", argv[0], argv[1]);
			}
		}
		else							
			chdir(getenv("HOME"));
		
		return 1;
	}

	if(strcmp(argv[0], "exit") == 0)	/* exit command */
		exit(0);

	if(strcmp(argv[0], "jobs") == 0){	/* List the running and stopped background jobs */
		listJobs();
		return 1;
	}

	if(strcmp(argv[0], "bg") == 0){		/* Change a stopped background job to a running background job */
		if(CMD_fgbgkill(argv, 1) < 0){
			fprintf(stderr, "*** Failed command: bg ***\n");
		}
		return 1;
	}

	if(strcmp(argv[0], "fg") == 0){		/* Change a stopped or running background job to a running in the foreground */
		if(CMD_fgbgkill(argv, 0) < 0){
			fprintf(stderr, "*** Failed command: fg ***\n");
		}
		return 1;
	}

	if(strcmp(argv[0], "kill") == 0){	/* Terminate a job */
		if(CMD_fgbgkill(argv, -1) < 0){
			fprintf(stderr, "*** Failed command: kill ***\n");
		}
		return 1;
	}

	if(strcmp(argv[0], "quit") == 0)
		_exit(0);
	
	if(strcmp(argv[0], "&") == 0)
		return 1;

	return 0;	/* Not a built-in command */
}

/* parse the command line and built the argv array */
int parseline(char *buf, char **argv){
	char *delim;	/* Points to first space delimiter */
	int argc;		/* Number of args */
	int bg;			/* Background job? */

	buf[strlen(buf)-1] = ' ';  /* Replace '\n' with space */
	while(*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	while((delim = strchr(buf, ' '))){
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while(*buf && (*buf == ' ')) /* Ignore spaces */
			++buf;
	}
	argv[argc] = NULL;

	if(argc == 0) /* ignore blank line */
		return 1;

	/* Should the job run in the background? */
	if((bg = (*argv[argc-1] == '&')) != 0){ /* & is separated from the command by a space */
		argv[--argc] = NULL;
	}
	else{
		int idx = strlen(argv[argc-1]) -1;
		if((bg = (argv[argc-1][idx] == '&')) != 0) /* & is not separated from the command by a space */
			argv[argc-1][idx] = '\0';
	}
	
	return bg;
}

int parsepipe(char *buf, char *tmpCMD, char **argv){
	char ptr[MAXLINE];

	/* Parse by '|' */
	for(int i=0; i<(int)strlen(buf); i++){
		if(buf[i] == '|'){
			for(int j=i+1; ; j++){
				if(buf[j] == ' ') continue;
				else{
					strcpy(tmpCMD, buf+j);
					break;
				}
			}
			ptr[i] = '\n';
			return parseline(ptr, argv);
		}
		ptr[i] = buf[i];
	}

	/* If last pipe, tmpCMD is NULL */
	tmpCMD[0] = '\0';
	return parseline(ptr, argv);
}

void makepipe(char **argv, char *buf){
	pid_t pid;
	int child_status;
	int fd[2];
	char *tmpARG[MAXARGS];
	char tmpCMD[MAXLINE];
	int bg;

	/* Make pipe */
	if(pipe(fd) < 0){
		fprintf(stderr, "*** Pipe error ***\n");
		_exit(0);
	}

	/* Fork */
	pid = fork();
	if(pid < 0){ /* fork error */
		fprintf(stderr, "*** Fork error ***\n");
		_exit(0);
	}
	else if(pid == 0){ /* child process */
		close(fd[0]); /* not used */
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		if(execvp(argv[0], argv) < 0){
			fprintf(stderr, "%s: Command not found.\n", argv[0]);
			_exit(0);
		}
	}
	else{ /* parent process */
		close(fd[1]); /* not used */
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);

		if(waitpid(pid, &child_status, WUNTRACED) < 0){
			fprintf(stderr, "*** Wait error ***\n");
			_exit(0);
		}
		
		bg = parsepipe(buf, tmpCMD, tmpARG);

		if(tmpCMD[0] == '\0'){ /* Last pipe */
			if(execvp(tmpARG[0], tmpARG) < 0){
				fprintf(stderr, "%s: Command not found.\n", argv[0]);
				_exit(0);
			}
		}
		else
			makepipe(tmpARG, tmpCMD);
	}
}

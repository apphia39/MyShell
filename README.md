# MyShell

## Project Name
  MyShell
  
## How to compile
  $ make

## How to run
  $ ./myshell


## About source code
#### 1. Show prompt  
#### 2. Read command (user input) 
#### 3. Parse command (delimiter is ' ')  
#### 4. Check if built-in command
##### (1) If not built-in command, fork
	 - Child process  : execute the command 
	 - Parent process : wait for the child process 
##### (2) If built-in command, just execute the command
	- cd         : navigate the directories 
	- exit       : terminate the shell 
	- jobs       : list the running and stopped background jobs
	- bg [job]   : change a stopped background job to a running background job
	- fg [job]   : change a stopped or running background job to a running foreground job 
	- kill [job] : terminate a job 
	
	+) Description about [job]
	: This description is applied to 'bg [job]', 'fg [job]', and 'kill [job]' commands.
	  [job] should be '%[number]' or '[number]'.
	  If [job] is '%[number]', number means the index of JOB 
	  Else if [job] is '[number]', number means the pid of JOB
	  Others are error 

#### 5. Check if pipe 
##### (1) If pipe, parse command (delimiter is '|') 
	- Child process  : execute each command which is separated by '|'  
		           output of one pipe should be the input of the next pipe 
		           (using recursive function 'make_pipe')  
	- Parent process : wait for the child process  
##### (2) If not pipe, same as #4. 

#### 6. Check if background 
##### (1) If background, parent process does not wait for the child process
##### (2) If foreground, parent process waits for the child process  


## About writer
  Author : 20181603, Minseon Kim  <br>
  Date   : 2021-05-13 ~ 2021-05-30  <br>

 <br>
##### Sogang Univ. System Programming - My Shell project

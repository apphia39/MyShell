# MyShell

## How to compile
```
$ make
```
<br>

## How to run
```
$ ./myshell
```
<br>

## About source code
### 1. Show prompt
<br>

### 2. Read command (user input)
<br>

### 3. Parse command (delimiter is ' ')
<br>

### 4. Check if built-in command
#### I. If not built-in command, fork 
	- Child process  : execute the command 
	- Parent process : wait for the child process
	
#### II. If built-in command, just execute the command <br>
	- cd         : navigate the directories 
	- exit       : terminate the shell 
	- jobs       : list the running and stopped background jobs
	- bg [job]   : change a stopped background job to a running background job
	- fg [job]   : change a stopped or running background job to a running foreground job 
	- kill [job] : terminate a job 
	
#### +) Description about [job]
	: This description is applied to 'bg [job]', 'fg [job]', and 'kill [job]' commands. 
	  [job] should be '%[number]' or '[number]'. 
	  If [job] is '%[number]', number means the index of JOB 
	  Else if [job] is '[number]', number means the pid of JOB 
	  Others are error  
<br>

### 5. Check if pipe 
#### I. If pipe, parse command (delimiter is '|') 
- Child process: <br>
  → execute each command which is separated by '|' <br>
  → output of one pipe should be the input of the next pipe <br>
  → (using recursive function 'make_pipe')  <br>

- Parent process: <br> 
  → wait for the child process  <br>
  
#### II. If not pipe, same as #4. 
<br>

### 6. Check if background 
#### I. If background, parent process does not wait for the child process
#### II. If foreground, parent process waits for the child process  
<br>

## About writer
- Author : 20181603, Minseon Kim 
- Date   : 2021-05-13 ~ 2021-05-30  

# MyShell

## Project Name
  MyShell - phase3 (background processing and new built-in commands are added)

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
 - Child process  : execute the command <br>
 - Parent process : wait for the child process <br>
##### (2) If built-in command, just execute the command
- cd   : navigate the directories <br>
- exit : terminate the shell <br>
- jobs : list the running and stopped background jobs <br>
- bg [job]   : change a stopped background job to a running background job <br>
- fg [job]   : change a stopped or running background job to a running foreground job <br>
- kill [job] : terminate a job <br>
+) Description about [job] <br>
: This description is applied to 'bg [job]', 'fg [job]', and 'kill [job]' commands.<br>
  [job] should be '%[number]' or '[number]'. <br>
  If [job] is '%[number]', number means the index of JOB <br>
  Else if [job] is '[number]', number means the pid of JOB <br>
  Others are error <br>

#### 5. Check if pipe 
##### (1) If pipe, parse command (delimiter is '|') <br>
         - Child process  : execute each command which is separated by '|'  <br>
		           output of one pipe should be the input of the next pipe <br> 
			   (using recursive function 'make_pipe')  <br>
         - Parent process : wait for the child process  <br>
##### (2) If not pipe, same as #4.  <br>

#### 6. Check if background 
##### (1) If background, parent process does not wait for the child process  <br>
##### (2) If foreground, parent process waits for the child process  <br>


## About writer
  Author : 20181603, Minseon Kim  <br>
  Date   : 2021-05-30  <br>

 <br>
##### Sogang Univ. System Programming - My Shell project

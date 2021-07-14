# MyShell

## Project Name
  MyShell - phase3 (background processing and new built-in commands are added)

## How to compile
  $ make

## How to run
  $ ./myshell


## About source code
  1. Show prompt  <Br><Br>
  2. Read command (user input) <Br><Br>
  3. Parse command (delimiter is ' ')  <Br><Br>
  4. Check if built-in command <Br>
  >(1) If not built-in command, fork <Br>
	>> - Child process  : execute the command <Br>
	>> - Parent process : wait for the child process <Br>
  >(2) If built-in command, just execute the command <Br>
	>> - cd   : navigate the directories <Br>
	>> - exit : terminate the shell <Br>
	>> - jobs : list the running and stopped background jobs <Br>
	>> - bg <job>   : change a stopped background job to a running background job <Br>
	>> - fg <job>   : change a stopped or running background job to a running foreground job <Br>
	>> - kill <job> : terminate a job <Br>
	<Br>
	>> +) Description about <job> <Br>
	>> : This description is applied to 'bg <job>', 'fg <job>', and 'kill <job>' commands. <Br>
	>>   <job> should be '%[number]' or '[number]'. <Br>
	>>   If <job> is '%[number]', number means the index of JOB <Br>
	>>   Else if <job> is '[number]', number means the pid of JOB <Br>
	>>   Others are error <Br>
 <Br>		
 5. Check if pipe <Br>
  >(1) If pipe, parse command (delimiter is '|') <Br>
       >> Child process  : execute each command which is separated by '|' <Br>
		           output of one pipe should be the input of the next pipe <Br> 
			   (using recursive function 'make_pipe') <Br>
       >> Parent process : wait for the child process <Br>
  >(2) If not pipe, same as #4. <Br>		
 <Br>
 6. Check if background <Br>
    >(1) If background, parent process does not wait for the child process <Br>
    >(2) If foreground, parent process waits for the child process <Br>


## About writer
  Author : 20181603, Minseon Kim <Br>
  Date   : 2021-05-30 <Br>

<Br>
##### Sogang Univ. System Programming - My Shell project

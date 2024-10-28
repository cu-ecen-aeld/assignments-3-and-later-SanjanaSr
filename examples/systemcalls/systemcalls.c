#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <syslog.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

    if(cmd==NULL){
       printf("ERROR: There is no input command");
       syslog(LOG_ERR,"ERROR: No input command");
       return false;
    }
    
    int ret_code = system(cmd);
    if(ret_code==-1){
      printf("ERROR: failed to execute system call: %s", strerror(errno));
      syslog(LOG_ERR,"ERROR:failed to execute system call %s",strerror(errno));
      return false;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    pid_t child_pid,wait_pid;
    bool exec_success = false;
    openlog("do_exec_func", 0, LOG_USER);
    
    child_pid = fork();
    if(child_pid==-1){
       syslog(LOG_ERR,"ERROR:fork call failed: %s\n",strerror(errno));
       exec_success=false;
    }
    
    else if(child_pid==0){
	    if(execv(command[0],command)==-1){
	    syslog(LOG_ERR,"ERROR:execv call failed:%s \n",strerror(errno));
	    }
	    exit(EXIT_FAILURE);
    }
    
    else{
    	int child_status;
    	wait_pid = waitpid(child_pid, &child_status,0);
    	if(wait_pid==-1){
    		syslog(LOG_ERR,"ERROR:wait failed: %s\n",strerror(errno));
       exec_success=false;	
    	}
    	
    	else if(WIFEXITED(child_status) && WEXITSTATUS(child_status) == EXIT_SUCCESS){
    		exec_success=true;
    	}
    	
    	else{
    		exec_success=false;
    	}
    
    }
    va_end(args);
    closelog();
    //return true;
    return exec_success;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    openlog("do_exec_redirect_func", 0, LOG_USER);

    // Open the output file
    int fd = open(outputfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        syslog(LOG_ERR, "ERROR: Failed to open file %s: %s", outputfile, strerror(errno));
        va_end(args);
        closelog();
        return false;
    }

    // Fork to create a child process
    pid_t child_pid = fork();
    bool exec_success = false;

    if (child_pid == -1) {  // Fork failed
        syslog(LOG_ERR, "ERROR: Fork call failed: %s", strerror(errno));
    } 
    else if (child_pid == 0) {  // Child process
        // Redirect standard output to the output file
        if (dup2(fd, STDOUT_FILENO) == -1) {
            syslog(LOG_ERR, "ERROR: Failed to redirect output: %s", strerror(errno));
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);

        // Execute command with execv
        if (execv(command[0], command) == -1) {
            syslog(LOG_ERR, "ERROR: execv call failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } 
    else {  // Parent process
        close(fd);  // Parent does not need the file descriptor

        // Wait for the child process to complete
        int child_status;
        pid_t wait_pid = waitpid(child_pid, &child_status, 0);
        
        if (wait_pid == -1) {
            syslog(LOG_ERR, "ERROR: waitpid call failed: %s", strerror(errno));
        } 
        else if (WIFEXITED(child_status) && WEXITSTATUS(child_status) == EXIT_SUCCESS) {
            exec_success = true;
        } 
        else {
            syslog(LOG_ERR, "ERROR: Child process terminated with failure.");
        }
    }

    va_end(args);
    closelog();
   // return true;
    return exec_success;   
}

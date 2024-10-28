#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main(int argc, char *argv[]){
    openlog("writer",LOG_PID | LOG_CONS, LOG_USER);//Open syslog 
    
//Validate the number of arguments
    if(argc!=3){
      syslog(LOG_ERR, "Error:Invalid no. of arguments. Expected %s <file> <str>",argv[0]);
      printf("Expected usage: %s <file><str>\n",argv[0]);
      exit(1);
     }
    
    const char *file =argv[1];
    const char *str = argv[0];
  
    FILE *file1 =fopen(file, "w"); //Open file to write
    if(file1==NULL){
    syslog(LOG_ERR,"ERROR:Unable to open file %s for writing: %s", file,strerror(errno));
    exit(1);
    }
    
    
    if(fprintf(file1, "%s", str) < 0){
      syslog(LOG_ERR,"Error:Unable to write in file %s:%s", file,strerror(errno));
      fclose(file1);
      exit(1);
    }

    //Log successful write
    syslog(LOG_DEBUG, "Writing %s to %s successful", str, file);
    
    fclose(file1);
    closelog();

    return 0;
}

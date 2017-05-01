// 
// File:   usbpanic.cpp
//
// Goal: main program, launched as daemon and in charge of launching actin on button press
//
// Author: Stéphane Mouton
//
// Created on 6 avril 2008, 21:35
//

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_NAME "usbpanic"
#define VERSION "2.5"
#define DEFAULT_CONFIGURATION_FILE "/etc/usbpanic.conf"

#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

#include "parameters.hpp"
#include "usbdriver.hpp"

// Global variables
Parameters parameters;
int notEnding=1;

static void daemonize(void){
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

void signalHandler(int signal){
    switch (signal){
        case SIGCHLD :
            while (0 < waitpid(-1, NULL, WNOHANG));
        break;
        case SIGHUP :
            syslog(LOG_INFO, "Caught SIGHUP, re-reading parameters\n");
            // re-read parameters from /etc/usbpanic.conf
            parameters.load((char *)DEFAULT_CONFIGURATION_FILE);
        break;
        case SIGINT  :
        case SIGQUIT :
        case SIGTERM :
            syslog(LOG_INFO, "Caught exit signal, exiting cleanly");
            notEnding=0;
        break;
        default:
            syslog(LOG_ERR, "Caught signal %d", signal);
        break;
    }
    
}

//
// Main function. Arguments are not used because /etc/usbpanic.conf is expected
//
int main(int argc, char** argv){
    pid_t pid;
    int exitStatus;
    unsigned int j;
    char interactiveMode=0;
    char helpMode=0;
    char displayVersion=0;
    char* fileName;
    struct sigaction sa;
    ButtonResult* result;
    notEnding=1;
    int option=0;
    int logopt = LOG_PID;

    fileName = (char *)DEFAULT_CONFIGURATION_FILE;

    /*
     * -d : debug (interactive mode, no daemonize)
     * -h : version
     * -h : usage
     * -f : alternate configuration file
     */
    while ((option = getopt (argc, argv, "dhvf:")) != -1)
        switch (option){
        case 'd':
            interactiveMode = 1;
            break;
        case 'h':
            helpMode = 1;
            break;
        case 'v':
            displayVersion = 1;
            break;
        case 'f':
            fileName = optarg;
            break;
        default:
            abort ();
        }

    if (displayVersion){
        printf("%s USB panic button manager, Version %s\n", argv[0], VERSION);
        exit(EXIT_SUCCESS);
    }

    if (helpMode){
        printf("usbpanic, version %s. Service to manage USB panic buttons with USB ID 1130:0202\n",VERSION);
        printf("Usage: usbpanic [OPTIONS]\n");
        printf("Options\n");
        printf("-h print this help and exit\n");
        printf("-v print version and exit\n");
        printf("-d debug mode, launch program in display mode (remain connected to console)\n");
        printf("-f FILE change configuration file (default is %s)\n",DEFAULT_CONFIGURATION_FILE);
        exit(EXIT_SUCCESS);
    }

    // Cleanly run the program as a daemon
    if (!interactiveMode) daemonize();

    // Display also syslog messages in console
    if (interactiveMode) logopt|=LOG_PERROR;

    // Open Syslog communication
    openlog( DAEMON_NAME, logopt, LOG_LOCAL5 );
    syslog( LOG_INFO, "Starting" );

    // read parameters from configuration file
    if(access(fileName,R_OK) == -1){
        syslog( LOG_ERR, "Unable to open configuration file %s",fileName );
        exit(EXIT_FAILURE);
    }
    parameters.load(fileName);
    
    // init USB communication
    USBDriver driver;
    if (!driver.init()){
        exitStatus=EXIT_FAILURE;
        syslog( LOG_ERR, "Unable to init USB sybsystem" );
    }else{
        exitStatus=EXIT_SUCCESS;

        // Configure signal handling to:
        // - avoid child zombie creation in asynchronous mode
        // - allow re-reading of configuration file
        // - handle signal and cleanly stop
        memset (&sa, 0, sizeof sa);
        sa.sa_flags = 0;
        // signalHandler will deal with the 3 cases described previously
        sa.sa_handler = signalHandler;
        sigemptyset(&(sa.sa_mask));
        if(sigaction(SIGCHLD, &sa, NULL) != 0){
            syslog(LOG_ERR, "Unable to perform sigaction on SIGCHLD");
            exit(EXIT_FAILURE);
        }        
        if(sigaction(SIGHUP, &sa, NULL) != 0){
            syslog(LOG_ERR, "Unable to perform sigaction on SIGHUP");
            exit(EXIT_FAILURE);
        }        
        if(sigaction(SIGTERM, &sa, NULL) != 0){
            syslog(LOG_ERR, "Unable to perform sigaction on SIGTERM");
            exit(EXIT_FAILURE);
        }        
        if(sigaction(SIGINT, &sa, NULL) != 0){
            syslog(LOG_ERR, "Unable to perform sigaction on SIGINT");
            exit(EXIT_FAILURE);
        }        
        if(sigaction(SIGQUIT, &sa, NULL) != 0){
            syslog(LOG_ERR, "Unable to perform sigaction on SIGQUIT");
            exit(EXIT_FAILURE);
        }        

        // OK, now that we have found the device and prepared communication, we wait for event
        while(notEnding)
        {
            result=driver.requestButtonStatus();            
            if (result->numberPressed){
                syslog( LOG_INFO, "%d Button(s) pressed on a total of %d",result->numberPressed,result->total);
                for (j=0; j<result->total; j++){
                    syslog( LOG_INFO, "Button[%d]=%d",j,result->value[j]);
                    if (result->value[j]){
                        // There is a script to execute for button j
                        syslog( LOG_INFO, "Attempting to execute %s", (char *)parameters.getScriptName(j));
                        if (parameters.getAsynchronous()) {
                            pid = fork();
                            if (pid < 0) {
                                exit(EXIT_FAILURE);
                            }
                            /* the child executes command and ends. */
                            if (pid == 0) {
                                system(parameters.getScriptName(j));
                                exit(EXIT_SUCCESS);
                            }
                        }else {
                            system(parameters.getScriptName(j));
                        }
                        // End script execution
                    }
                }
            }
            usleep(parameters.getScanPeriod());
        }
    }
    driver.close();
    syslog( LOG_NOTICE, "terminated" );
    closelog();
    return (exitStatus);
}


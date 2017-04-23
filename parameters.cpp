/* 
 * File:   parameters.hpp
 *
 * Goal: Parameters management using iniparser library
 *
 * Author: Stéphane Mouton
 *
 * Created on 19 avril 2008, 18:23
 */

#include "parameters.hpp"
#include <syslog.h>

extern "C" {
#include "iniparser/src/iniparser.h"
}
    
Parameters::Parameters(){
    script="";
    scanperiod=0;
    asynchronous=1;
}


int Parameters::getScanPeriod(void){ return scanperiod;}
unsigned char Parameters::getAsynchronous(void){ return asynchronous;}

const char * Parameters::getScriptName(unsigned int index){
    dictionary * ini ;
    ini = iniparser_load(parameterFileName.c_str());
    if (ini==NULL) {
            syslog( LOG_ERR, "cannot re-read file: %s\n", parameterFileName.c_str());
            return script.c_str();
    }
    
    /* try to get the string for scriptX (where X is index passed as parameter)
       else return default script */
    sprintf(scriptParameter,"action:script%d",index);
    syslog( LOG_INFO, "looking for script parameter: %s \n", scriptParameter);
    scriptName = iniparser_getstring(ini, scriptParameter, (char *)script.c_str());
    iniparser_freedict(ini);

    return scriptName.c_str();
}


int Parameters::load(char * fileName){
    parameterFileName=fileName;
    
    dictionary	*	ini ;    
    ini = iniparser_load(fileName);
    if (ini==NULL) {
            syslog( LOG_ERR, "cannot parse file: %s\n", fileName);
            return(0);
    }
    
    /* Get global attributes */
    scanperiod = iniparser_getint(ini, "global:scanperiod", 1000);
    asynchronous = iniparser_getboolean(ini, "global:asynchronous", 1);
    
    /* Get Action attributes */
    script = iniparser_getstring(ini, "action:defaultscript", (char *)"/bin/true");

    syslog( LOG_INFO, "scanperiod: %d\n", scanperiod);
    syslog( LOG_INFO, "defaultscript: %s\n", script.c_str());
    syslog( LOG_INFO, "asynchronous: [%d]\n", asynchronous);    
    iniparser_freedict(ini);
    return(1);
}

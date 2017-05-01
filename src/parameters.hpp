/* 
 * File:   parameters.hpp
 *
 * Goal: Header for parameters management
 *
 * Author: Stéphane Mouton
 *
 * Created on 19 avril 2008, 18:23
 */

#ifndef _PARAMETERS_HPP
#define	_PARAMETERS_HPP

#define DEFAULT_SCAN_PERIOD 1000
#define DEFAULT_LAUNCH_MODE 1  /* asynchronous launch */
#define DEFAULT_SCRIPT      "/bin/true"

#include <string>
using namespace std ;
/*
 * wrap reading of parameters
 */
class Parameters {
private:
    string parameterFileName;
    char scriptParameter[20];
    string scriptName;
    // [Global]
    int scanperiod;
    unsigned char asynchronous;    
    // [Action]
    string script;

public:
    Parameters();
    int load(char *);
    int getScanPeriod(void);
    unsigned char getAsynchronous(void);
    const char * getScriptName(unsigned int index);
};

#endif	/* _PARAMETERS_HPP */

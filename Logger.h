/* 
 * File:   Logger.h
 * Author: knoppix
 *
 * Created on March 27, 2015, 8:54 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "constants.h"

class Logger {
public:
    static Logger* getLogger();
    static void init(std::string path);
    static void destroy();
    
    Logger(std::string path);
    virtual ~Logger();
    
    void escribir(int type,std::string mensaje);
private:
    int log_file;
    
    static Logger* instancia;

};

#endif	/* LOGGER_H */


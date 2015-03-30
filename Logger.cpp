/* 
 * File:   Logger.cpp
 * Author: knoppix
 * 
 * Created on March 27, 2015, 8:54 PM
 */

#include "Logger.h"

Logger* Logger::instancia = NULL;

Logger::Logger(std::string path){
    log_file = open(path.c_str(),O_RDWR|O_APPEND|O_CREAT,0777);
}

Logger::~Logger() {
    close(log_file);
}

Logger* Logger::getLogger(){
    if (instancia == NULL ) {
        std::string msj = "Logger no iniciado";
        write(STDERR_FILENO,msj.c_str(),sizeof(char)*msj.size());
    }
    return instancia;
}

void Logger::init(std::string path){
    instancia = new Logger(path);
}

void Logger::destroy(){
    delete instancia;
}

void Logger::escribir(int type,std::string mensaje){ //TODO timestamp y distintos tipos de escribir (error, msj)
    int res = flock(log_file,LOCK_EX);
    if (res != 0) {
        perror("Logger: No se pudo tomar el lock.");
        return;
    }
    std::string msj;
    if (type == ERROR) {
        msj = "ERROR: ";
    } else if (type == MSJ) {
        msj = "MESSAGE: ";
    }
    msj = msj + mensaje;
    write(log_file,msj.c_str(),sizeof(char)*msj.size());
    res = flock(log_file,LOCK_UN);
    if (res != 0) {
        perror("Logger: No se pudo devolver el lock.");
        return;
    }
    return;    
}


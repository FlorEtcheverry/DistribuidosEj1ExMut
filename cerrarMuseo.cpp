/* 
 * File:   cerrarMuseo.cpp
 * Author: knoppix
 *
 * Created on March 30, 2015, 12:23 AM
 */

#include <cstdlib>
#include <sys/shm.h>
#include "Semaforo.h"
#include "Logger.h"

using namespace std;

/*
 * cierra museo (cambia la mem compartida del museo a cerrado y libera el mutex)
 */
int main(int argc, char** argv) {
    
    Logger::init(PATH_LOG,argv[0]);
    
    (Logger::getLogger())->escribir(MSJ,"Cerrando museo...");
    //obtiene el semaforo
    Semaforo mutex = Semaforo(PATH_IPC.c_str());
    
    //obtiene la mem compartida
    key_t key = ftok(PATH_IPC.c_str(),SHM);
    int shm_id = shmget(key,sizeof(MUSEO),0666);
    if (shm_id == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo obtener la memoria compartida al cerrar museo.");
        Logger::destroy();
        exit(1);
    }
    MUSEO* museo_shm = static_cast<MUSEO*>(shmat(shm_id,0,0));
    if (museo_shm == (MUSEO*)(-1)) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo attachear a la memoria compartida al cerrar museo.");
        Logger::destroy();
        exit(1);
    }
    
    mutex.p();
    museo_shm->abierto = 0;
    mutex.v();

    //desatacheo de la memoria
    int res = shmdt(museo_shm);
    if (res < 0) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo desatachear la memoria compartida al cerrar museo.");
        Logger::destroy();
        exit(1);
    }
    (Logger::getLogger())->escribir(MSJ,"El museo se ha cerrado.");
    Logger::destroy();
    return 0;
}


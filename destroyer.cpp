/* 
 * File:   destroyer.cpp
 * Author: knoppix
 *
 * Created on March 29, 2015, 10:53 PM
 */

#include <cstdlib>
#include <sys/shm.h>
#include "Semaforo.h"
#include "Logger.h"

using namespace std;

/*
 * destruye los ipc
 */
int main(int argc, char** argv) {

    Semaforo mutex = Semaforo(PATH_IPC.c_str());
    mutex.destroy(); //TODO errores?
    
    key_t key = ftok(PATH_IPC.c_str(),SHM);
    int shm_id = shmget(key,sizeof(MUSEO),0666);
    if (shm_id == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo obtener la memoria compartida para destruirla.");
        exit(1);
    }
    int res = shmctl(shm_id,IPC_RMID,NULL);
    if (res == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo destruir la memoria compartida.");
        exit(1);
    }
    return 0;
}


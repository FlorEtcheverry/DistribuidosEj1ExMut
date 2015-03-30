/* 
 * File:   puerta.cpp
 * Author: knoppix
 *
 * Created on March 29, 2015, 6:35 PM
 */

#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "Logger.h"
#include "Semaforo.h"
#include "constants.h"

using namespace std;

int nroRandom() {
    srand (time(NULL));
    int res = rand() % MAX_RAND + 1;
    return res;
}

/*
 * 
 */
int main(int argc, char** argv) { //parametros: nro_puerta,nro_personas_por_puerta
    
    static char* puerta = argv[1];
    int nro_pers_puerta = atoi(argv[2]);
    (Logger::getLogger())->escribir(MSJ,string("Puerta numero ")+puerta+" creada.");
    (Logger::getLogger())->escribir(MSJ,string("Por la puerta nro ")+puerta+" ingresan/egresan "+nro_pers_puerta+" personas.");
    
    //obtiene el semaforo
    Semaforo mutex = Semaforo(PATH_IPC.c_str());
    
    //obtiene la mem compartida
    key_t key = ftok(PATH_IPC.c_str(),SHM);
    int shm_id = shmget(key,sizeof(MUSEO),0666);
    if (shm_id == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo obtener la memoria compartida en la puerta "+puerta+".");
        exit(1);
    }
    MUSEO* museo_shm = static_cast<MUSEO*>(shmat(shm_id,0,0));
    if (museo_shm == (MUSEO*)(-1)) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo attachear a la memoria compartida la puerta "+puerta+".");
        Logger::destroy();
        exit(1);
    }
    
    for (int i = 0; i < nro_pers_puerta; i++) {
        
        int rand = nroRandom();
        mutex.p(); //TODO errores?
        if (!(museo_shm->abierto)) { //si esta cerrado
            (Logger::getLogger())->escribir(MSJ,string("Museo cerrado. Salen ")+(museo_shm->cant_personas)+" por la puerta "+puerta+".");
            while ((museo_shm->cant_personas) > 0) { //salen todas las personas
                (museo_shm->cant_personas)--;
            }
        } else if (rand > MID_RAND && museo_shm->cant_personas < museo_shm->max_personas && museo_shm->abierto) {//entra
            (Logger::getLogger())->escribir(MSJ,string("Entró una persona por la puerta ")+puerta+".");
            (museo_shm->cant_personas)++;
        } else { //sale
            if (museo_shm->cant_personas > 0) {
                (Logger::getLogger())->escribir(MSJ,string("Una persona salió por la puerta ")+puerta+".");
                (museo_shm->cant_personas)--;
            } else {
                (Logger::getLogger())->escribir(MSJ,string("Puerta ")+puerta+": No pueden salir mas personas porque el museo esta vacio.");
            }
        }
        mutex.v();
        //sleep(nroRandom()%10);
    }
    
    //desatacheo de la memoria
    int res = shmdt(museo_shm);
    if (res < 0) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo desatachear la memoria compartida en la puerta "+puerta+".");
        Logger::destroy();
        exit(1);
    }
    Logger::destroy();
    return 0;
}


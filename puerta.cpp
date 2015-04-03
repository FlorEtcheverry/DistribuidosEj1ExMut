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

void initRandom(int nro_puerta) {
    srand (time(NULL)+nro_puerta);
    for (int i=0;i<100;i++){
        rand();
    }
}

int nroRandom() {
    int res = (rand() % MAX_RAND) + 1;
    return res;
}

/*
 * 
 */
int main(int argc, char** argv) { //parametros: nro_puerta,nro_personas_por_puerta
    
    Logger::init(PATH_LOG,argv[0]);
    
    static char* puerta = argv[1];
    int nro_puerta = atoi(argv[1]);
    static char* pers_puerta = argv[2];
    int nro_pers_puerta = atoi(argv[2]);
    (Logger::getLogger())->escribir(MSJ,string("Puerta numero ")+puerta+" creada.");
    (Logger::getLogger())->escribir(MSJ,string("Por la puerta nro ")+puerta+" ingresan/egresan "+pers_puerta+" personas.");
    
    
    //obtiene el semaforo
    Semaforo mutex = Semaforo(PATH_IPC.c_str());
    
    //obtiene la mem compartida
    key_t key = ftok(PATH_IPC.c_str(),SHM);
    int shm_id = shmget(key,sizeof(MUSEO),0666);
    if (shm_id == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo obtener la memoria compartida en la puerta "+puerta+".");
        Logger::destroy();
        exit(1);
    }
    MUSEO* museo_shm = static_cast<MUSEO*>(shmat(shm_id,0,0));
    if (museo_shm == (MUSEO*)(-1)) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo attachear a la memoria compartida la puerta "+puerta+".");
        Logger::destroy();
        exit(1);
    }
    
    initRandom(nro_puerta);
    for (int i = 0; i < nro_pers_puerta; i++) {
        
        int rand = nroRandom();
        char total_personas[MAX_DIG_TOTAL_PERSONAS];
        mutex.p();
        if (!(museo_shm->abierto)) { //si esta cerrado
            sprintf(total_personas,"%d",(museo_shm->cant_personas));
            (Logger::getLogger())->escribir(MSJ,string("Museo cerrado. Salen ")+total_personas+" por la puerta "+puerta+".");
            while ((museo_shm->cant_personas) > 0) { //salen todas las personas
                (museo_shm->cant_personas)--;
            }
        } else if ((rand > MID_RAND) && (museo_shm->cant_personas < museo_shm->max_personas) && (museo_shm->abierto)) {//entra
            (museo_shm->cant_personas)++;
            sprintf(total_personas,"%d",(museo_shm->cant_personas));
            (Logger::getLogger())->escribir(MSJ,string("Entró una persona por la puerta ")+puerta+". Ahora hay "+total_personas+" personas en el museo.");
        } else { //sale
            if (museo_shm->cant_personas > 0) {
                (museo_shm->cant_personas)--;
                sprintf(total_personas,"%d",(museo_shm->cant_personas));
                (Logger::getLogger())->escribir(MSJ,string("Una persona salió por la puerta ")+puerta+". Ahora hay "+total_personas+" personas en el museo.");
            } else {
                (Logger::getLogger())->escribir(MSJ,string("No pueden salir mas personas por la puerta ")+puerta+" porque el museo esta vacio.");
            }
        }
        mutex.v();
        int d = nroRandom()%7;
        char dd[9];
        sprintf(dd,"%d",d);
        (Logger::getLogger())->escribir(MSJ,string("Puerta ")+puerta+" yendo a dormir "+dd+"segs .");
        sleep(d);
        (Logger::getLogger())->escribir(MSJ,string("Puerta ")+puerta+" despertó.");
    }
    (Logger::getLogger())->escribir(MSJ,string("No hay más personas que quieran entrar/salir del museo por la puerta ")+puerta+".");
    
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


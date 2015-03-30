/* 
 * File:   main.cpp
 * Author: knoppix
 *
 * Created on March 27, 2015, 8:31 PM
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

/*
 * carga los datos iniciales de config desde el archivo
 */
void cargarConfig(MUSEO* museo_shm,int* cant_visitantes,int* cant_puertas){
    std::ifstream conf_file(SHM_INIT_FILE.c_str());
    if (!conf_file.is_open()) {
        (Logger::getLogger())->escribir(ERROR," No se pudo abrir el archivo de conf para inicializar la memoria compartida.");
        shmdt(museo_shm);
        Logger::destroy();
        exit(1);
    }
    //abierto(1 o 0) ; cant_personas ; max_personas ; cant_visitantes ; cant_puertas (por linea)
    std::string linea;
    
    getline(conf_file,linea); //TODO: chequeo de errores
    (museo_shm->abierto) = atoi(linea.c_str());
    if (museo_shm->abierto) {
        Logger::getLogger()->escribir(MSJ,string("Inicializado museo abierto "));
    } else {
        Logger::getLogger()->escribir(MSJ,string("Inicializado museo cerrado "));
    }
    
    getline(conf_file,linea);
    (museo_shm->cant_personas) = atoi(linea.c_str());
    Logger::getLogger()->escribir(MSJ,string("con ")+linea.c_str()+" personas dentro.");
    
    getline(conf_file,linea);
    (museo_shm->max_personas) = atoi(linea.c_str());
    Logger::getLogger()->escribir(MSJ,string("La maxima cantidad de personas permitidas en el museo es ")+linea.c_str()+".");
    
    getline(conf_file,linea);
    *cant_visitantes = atoi(linea.c_str());
    Logger::getLogger()->escribir(MSJ,string("Van a ingresar/egresar ")+linea.c_str()+" personas por puerta.");
    
    getline(conf_file,linea);
    *cant_puertas = atoi(linea.c_str());
    Logger::getLogger()->escribir(MSJ,string("Hay")+linea.c_str()+" puertas.");
        
    conf_file.close(); //TODO: chequeo de errores
}

/*
 * crea los ipcs necesarios, carga datos iniciales desde arch de config y lanza los procesos
 */
int main(int argc, char** argv) {
  
    Logger::init(PATH_LOG);
    
    /*crear la mem compartida*/ //TODO memoria compartida como clase?
    key_t key = ftok(PATH_IPC.c_str(),SHM);
    int shm_id = shmget(key,sizeof(MUSEO),IPC_CREAT|IPC_EXCL|0666);
    if (shm_id == -1) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo crear la memoria compartida.");
        Logger::destroy();
        exit(1);
    }
    Logger::getLogger()->escribir(MSJ,"Creada memoria compartida.");
    
    /*inicializar mem compartida (att,read,det) y cargar parametros de conf*/
    MUSEO* museo_shm = static_cast<MUSEO*>(shmat(shm_id,0,0));
    if (museo_shm == (MUSEO*)(-1)) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo inicializar la memoria compartida.");
        Logger::destroy();
        exit(1);
    }
    
    int cant_visitantes;
    int cant_puertas;
    cargarConfig(museo_shm,&cant_visitantes,&cant_puertas);
    
    int res = shmdt(museo_shm);
    if (res < 0) {
        (Logger::getLogger())->escribir(ERROR,std::string(strerror(errno))+" No se pudo desatachear la memoria compartida al inicializar.");
        Logger::destroy();
        exit(1);
    }
    
    /*inicializar mutex*/
    Semaforo mutex = Semaforo(PATH_IPC.c_str());
    mutex.init(MUSEO_INICIAL);
    Logger::getLogger()->escribir(MSJ,"Inicializado el mutex.");
    
    /*lanzar procesos*/
    int child_pid;
    static char nro_puerta[MAX_DIG_PUERTA];
    static char nro_personas_por_puerta[MAX_DIG_PERS_POR_PUERTA];
    
    sprintf(nro_personas_por_puerta,"%d\n",cant_visitantes);
    
    for (int i=0;i<cant_puertas;i++) {
        sprintf(nro_puerta,"%d\n",i);
        (Logger::getLogger())->escribir(MSJ,string("Creando puerta numero ")+nro_puerta+".");
        child_pid = fork();
        if (child_pid < 0) {
            (Logger::getLogger())->escribir(ERROR,string(" No se pudo crear la puerta ")+nro_puerta+".");
            shmdt(museo_shm);
            Logger::destroy();
            exit(1);
        }
        if (child_pid == 0) { //hijo-puerta
            execlp(PUERTA_EXE,nro_puerta,nro_personas_por_puerta,(char*) 0);
            (Logger::getLogger())->escribir(ERROR,string(" No se pudo ejecutar la puerta ")+nro_puerta+".");
            shmdt(museo_shm);
            Logger::destroy();
            exit(1);
        }
    }
    (Logger::getLogger())->escribir(ERROR,string("Se han creado todas las puertas."));
    
    Logger::destroy();
    return 0;
}


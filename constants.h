/* 
 * File:   constants.h
 * Author: knoppix
 *
 * Created on March 28, 2015, 6:52 PM
 */

#ifndef CONSTANTS_H
#define	CONSTANTS_H

static const int MUSEO_INICIAL = 0; //empieza en 0 -no entra nadie hasta q alguien abra el museo-

static const std::string PATH_IPC = "/home/knoppix/AlvarezEtcheverry/Ejercicio1/v1";
static const std::string SHM_INIT_FILE = "/home/knoppix/AlvarezEtcheverry/Ejercicio1/v1/museo_inicial.conf";

static const int ERROR = 1;
static const int MSJ = 2;

static const int SEM = 100;
static const int SHM = 200;

static const char* PUERTA_EXE = "./puerta";

static const int MAX_DIG_PERS_POR_PUERTA = 11; //10 digitos mas el fin de string
static const int MAX_DIG_PUERTA = 11;

static const int MAX_RAND = 100;
static const int MID_RAND = 50;

typedef struct {
    int cant_personas;
    int abierto;
    int max_personas;
} MUSEO;
#endif	/* CONSTANTS_H */


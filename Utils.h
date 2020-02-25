#ifndef _Utils_H
#define _Utils_H

/* Ceci est le fichier de configs et de variables globales */


// Clefs pour les files de messages
#define clefileProcEntrants 345678
#define clefileProcExecution 346789

#define nombreTours 40 // Nombre de tours maximum, utile pour le debug
#define prioriteMax 10  // Priorite maximale qu'un processus peut avoir

typedef unsigned int myPid_t;

typedef struct { // Processus à Executer
    long priorite;
    myPid_t pid;
    int tempsExec;
    unsigned int dateSoumission;
} tProcess;


#endif
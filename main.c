#include "Utils.h"
#include "CPU.h"
#include "TableAllocation.h"
#include "GenerateurProc.h"
#include "MySemaphore.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

int main (int argc, char* argv[]) {
    int pas = 1;
    int dureeQuantum = 1;
	printf("Nombre  d’arguments  passes  au  programme  :  %d\n", argc);
	for(int i = 0 ; i< argc ; i ++) {
        //printf("%s\n", argv[i]);
		if(strcmp(argv[i], "-p") == 0) { // On recupere le pas
            pas = atoi(argv[i+1]);
            i++;
        }
        else if(strcmp(argv[i], "-d") == 0) { // On recupere la duree d'un quantum
            dureeQuantum = atoi(argv[i+1]);
            i++;
        }
	}	
    printf("[Config] Pas : %d\n", pas);
    printf("[Config] Duree quantum : %d\n", dureeQuantum);

    // Initialisation des nombres aléatoires, on passe le temps pour ne pas avoir la même graine plusieurs fois
    srand(time(NULL));

    // fermeture des file de messages si elles sont déjà ouvertes (par exemple si le programme crash)
    int fileProcEntrantsId;
    int fileProcExecutionId;
    if((fileProcEntrantsId = msgget(clefileProcEntrants, IPC_CREAT | 0750)) != -1)
		msgctl(fileProcEntrantsId, IPC_RMID, NULL); 
	if((fileProcExecutionId = msgget(clefileProcExecution, IPC_CREAT | 0750)) != -1)
		msgctl(fileProcExecutionId, IPC_RMID, NULL); 

	if ((fileProcEntrantsId = msgget(clefileProcEntrants, IPC_CREAT | IPC_EXCL | 0750)) == -1)
    	perror("Pb msgget de la file des procs entrants - main \n");

	if ((fileProcExecutionId = msgget(clefileProcExecution, IPC_CREAT | IPC_EXCL | 0750)) == -1)
    	perror("Pb msgget de la file des procs a executer - main \n");

    /*****
    * Création des sémaphores
    ******/
    long semid;
    if ((semid = initsem(SKEY)) < 0)    /* Création d'un ensemble de sémaphore */
        exit(1);

    /*****
    * Lecture du fichier pour generer les processus de bases
    ******/
    readFile();

    /*****
    * Lancement du thread qui simule le CPU
    ******/
	pthread_t CPU_thread;
	if(pthread_create(&CPU_thread, NULL, start_cpu, (void*)dureeQuantum) == -1)
		return -1;

    /*****
    * Lancement du thread qui simule la table d'allocations
    ******/
    pthread_t alloc_thread;
	if(pthread_create(&alloc_thread, NULL, start_alloc, (void*)pas) == -1)
		return -1;

    /*****
    * Attente de la fin des threads
    ******/
    if (pthread_join(CPU_thread, NULL)) {
		perror("pthread_join : CPU");
		return EXIT_FAILURE;
    }

    if (pthread_join(alloc_thread, NULL)) {
		perror("pthread_join : alloc");
		return EXIT_FAILURE;
    }
    // fermeture des file de messages	
	msgctl(fileProcExecutionId, IPC_RMID, NULL); 
	msgctl(fileProcEntrantsId, IPC_RMID, NULL);

    // fermeture des sémaphores
    semctl(semid, 0, IPC_RMID);
    semctl(semid, 1, IPC_RMID); 

    printf("Fin du super CPU \n");
	return 0;
}
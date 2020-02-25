#include "GenerateurProc.h"

#include "Utils.h"
#include "MySemaphore.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void readFile() {
	/******
	* Recuperation des files de messages
	*******/
	if ((_fileProcEntrantsId = msgget(clefileProcEntrants, IPC_CREAT | 0750)) == -1)
    	perror("Pb msgget de la file des procs entrants - generateurProc\n");

	FILE * fp;
   	fp = fopen ("proc.txt", "r+");
   	if(fp == NULL) {
   		printf("Erreur lors de l'ouverture du fichier proc.txt\n");
   		return;
   	}
   	int tempsExec = 0;
   	int priorite = 0;
   	while(fscanf(fp, "%d %d", &tempsExec, &priorite) != EOF) {
		tProcess tempProc;
	    tempProc.priorite = priorite;
	    tempProc.pid = 0;
	    tempProc.tempsExec = tempsExec;
	    int test;
		if ((test = msgsnd(_fileProcEntrantsId, &tempProc, sizeof(tProcess) - sizeof(long), 0)) == -1) {
		    printf("Erreur de la création d'un proc %d\n",test);
		    exit(1);
		}
		printf("Création d'un nouveau processus\n");
   	}
   	fclose(fp);
}

void soumission() {
	if(rand()%100 < 20) {
		tProcess tempProc;
	    tempProc.priorite = (rand()%10)+1; // Priorité entre 1 et 10
	    tempProc.pid = 0;
	    tempProc.tempsExec = (rand()%5)+1;
	    int test;
		if ((test = msgsnd(_fileProcEntrantsId, &tempProc, sizeof(tProcess) - sizeof(long), 0)) == -1) {
		    printf("Erreur de la création d'un proc %d\n",test);
		    exit(1);
		}
		printf("Soumission d'un nouveau processus de priorite: %ld et de tempsExec: %d\n", tempProc.priorite, tempProc.tempsExec);
	}
	V(0);
}
#include "CPU.h"

#include "Utils.h"
#include "GenerateurProc.h"
#include "MySemaphore.h"
#include "TableAllocation.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* Fonction principale */
void* start_cpu(void* dureeQuantum) {
	printf("Lancement du thread CPU\n");
	/*****
	* Récuperation des files de messages
	******/
	if ((_fileProcExecutionId = msgget(clefileProcExecution, IPC_CREAT | 0750)) == -1)
    	perror("Pb msgget de la file des procs a executer : CPU \n");

	if ((_fileProcEntrantsId = msgget(clefileProcEntrants, IPC_CREAT | 0750)) == -1)
    	perror("Pb msgget de la file des procs entrants \n");

	for(int quantum = 1; quantum < nombreTours; quantum++) {
		sleep(1);
		printf("=== Quantum : %d===\n", quantum);
		soumission();
		P(1);
		printf(" Priorite courante : %d\n", _prioriteCourante);
		/*****
		* Récupération du processus a executer
		******/
    	tProcess tempProc;
    	tempProc.pid = 0;

    	int tempPriorite = _prioriteCourante;
    	int count = 1;
    	while(!tempProc.pid) { // On parcourt les files tant quelles sont vides
		    msgrcv(_fileProcExecutionId, &tempProc, sizeof(tProcess) - sizeof(long), tempPriorite, IPC_NOWAIT);

			if(!tempProc.pid) {
				printf(" La file %d est vide\n", tempPriorite);
			}
			tempPriorite = (tempPriorite+1)%11;
			if(tempPriorite == 0)
				tempPriorite++;
			count++;
		    if(count == 10) {
		    	break;
		    }
		}
		if(!tempProc.pid) // Si toutes les files sont vides on passe au prochain quantum
			continue;
		/*****
		* Execution du processus
		******/
		printf(" Execution du processus n°:%d (%d)\n", tempProc.pid, tempProc.tempsExec);
		tempProc.tempsExec -= (int)dureeQuantum;
		/*****
		* Retiration du processus 
		******/
		if(tempProc.tempsExec <= 0) {
			printf(" Processus %d a finit son execution\n", tempProc.pid);
			continue;
		} 
		printf(" Retirement du processus %d\n", tempProc.pid);
		// On le remet dans la file des processus entrants
	    int test;
		if ((test = msgsnd(_fileProcEntrantsId, &tempProc, sizeof(tProcess) - sizeof(long), 0)) == -1) {
		    printf(" Erreur de la création d'un proc %d : CPU\n",test);
		    exit(1);
		}
	}
	printf("Fin thread CPU\n");
	return (void*)0;
}
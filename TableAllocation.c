#include "TableAllocation.h"

#include "Utils.h"
#include "MySemaphore.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void addProcess(tProcess tempProc) {
	if(tempProc.pid == 0) { 	// Nouveau processus
		tempProc.pid = _pidCourant;
		_pidCourant++;
	}
	else { 						// Processus retirer du CPU
		if(tempProc.priorite < prioriteMax)
			tempProc.priorite++;
	}

	printf("Ajout du processus %d dans la file d'execution de priorite : %ld\n", tempProc.pid, tempProc.priorite);
	if (msgsnd(_fileProcExecutionId, &tempProc, sizeof(tProcess) - sizeof(long), 0) == -1) {
	    perror("Erreur d'ajout dans la file d'execution\n");
	    exit(1);
	}
}

void* start_alloc(void* pas) {
	printf("Lancement du thread de la table d'allocations\n");
	// Initialisation
	_pidCourant = 1;
	_prioriteCourante = 1;
	/*****
	* Récuperation des files de messages
	******/
	if ((_fileProcEntrantsId = msgget(clefileProcEntrants, IPC_CREAT | 0750)) == -1)
    	perror("Pb msgget de la file des procs entrants \n");

	if ((_fileProcExecutionId = msgget(clefileProcExecution, IPC_CREAT | 0750)) == -1)
    	perror("Pb msgget de la file des procs a executer \n");

    char done = 0;
    int currentPas = 0;
    while(done != 4) { // Si done == 2, ca veut dire qu'on a plus de nouveau proc ni de proc en cours
    	done++;
    	tProcess tempProc;
    	P(0);
    	do { 
    		// Tant qu'il y a des processus en attente on les places dans la file d'execution
    		tempProc.tempsExec = -2;
		    msgrcv(_fileProcEntrantsId, &tempProc, sizeof(tProcess) - sizeof(long), 0, IPC_NOWAIT);
		    if(tempProc.tempsExec != -2) {
		    	addProcess(tempProc);
		    	done = 0;
		    }
		} while(tempProc.tempsExec != -2);
		if((currentPas % (int)pas) == 0) {
			// On calcule la nouvelle priorite
			computeNewPriority();
		}
		currentPas++;
		V(1);
    }
    printf("Fin du thread qui gere les allocations\n");
    return (void*)0;
}
void computeNewPriority() {
	int random = rand()%100;
	int prioritiesPerc[10] = {26,20,16,12,9,7,5,3,2,1};
	int currentPriorityPerc = prioritiesPerc[0];
	for(int i = 1; i < 11; i++) {
		if(random < currentPriorityPerc) {
			_prioriteCourante = i;
			return;
		}
		currentPriorityPerc += prioritiesPerc[i];
	}
}
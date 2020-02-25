#ifndef _TableAllocation_H
#define _TableAllocation_H

#include "Utils.h"

int _prioriteCourante;
myPid_t _pidCourant;
int _fileProcExecutionId;
int _fileProcEntrantsId;

void* start_alloc(void* t);
void computeNewPriority();

#endif
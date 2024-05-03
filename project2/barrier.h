#ifndef BARRIER_H
#define BARRIER_H

#include <omp.h>

extern omp_lock_t Lock;
extern volatile int NumInThreadTeam;
extern volatile int NumAtBarrier;
extern volatile int NumGone;

void InitBarrier(int n);
void WaitBarrier();

#endif

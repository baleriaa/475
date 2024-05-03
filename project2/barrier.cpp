#include "barrier.h"

omp_lock_t Lock;
volatile int NumInThreadTeam;
volatile int NumAtBarrier;
volatile int NumGone;

void InitBarrier(int n) {
    NumInThreadTeam = n;
    NumAtBarrier = 0;
    omp_init_lock(&Lock);
}

void WaitBarrier() {
    omp_set_lock(&Lock);
    NumAtBarrier++;
    if (NumAtBarrier == NumInThreadTeam) {
        NumGone = 0;
        NumAtBarrier = 0;
        while (NumGone != NumInThreadTeam - 1);
        omp_unset_lock(&Lock);
        return;
    }
    omp_unset_lock(&Lock);

    while (NumAtBarrier != 0);  // This waits for the nth thread to arrive

    #pragma omp atomic
    NumGone++;  // This flags how many threads have returned
}

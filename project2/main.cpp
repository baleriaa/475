#include <omp.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "barrier.h"

int NowYear = 2024;
int NowMonth = 0; 

float NowPrecip;   
float NowTemp;     
float NowHeight = 5.0;
int NowNumDeer = 10; 
int NowNumDragons = 1; 
bool headerPrinted = false;

unsigned int seed = 0; 
const float GRAIN_GROWS_PER_MONTH = 12.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;
const float ONE_DRAGON_EATS_PER_MONTH = 0.5;

const float AVG_PRECIP_PER_MONTH = 7.0;
const float AMP_PRECIP_PER_MONTH = 6.0;
const float RANDOM_PRECIP = 2.0;

const float AVG_TEMP = 60.0;
const float AMP_TEMP = 20.0;
const float RANDOM_TEMP = 10.0;

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;

float SQR(float x) {
    return x * x;
}

float Ranf(unsigned int *seedp, float min, float max) {
    float r = (float) rand_r(seedp); 
    return (r / (float) RAND_MAX) * (max - min) + min;
}

void InitBarrier(int n);
void WaitBarrier();

void UpdateWeather() {
    float ang = (30.0*(float)NowMonth + 15.0) * (M_PI / 180.0);

    float temp = AVG_TEMP - AMP_TEMP * cos(ang);
    NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
    NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
    if (NowPrecip < 0.) NowPrecip = 0.;
}

void Deer() {
    while (NowYear < 2030) {
        int nextNumDeer = NowNumDeer;
        int carryingCapacity = (int)(NowHeight);

        if (nextNumDeer < carryingCapacity)
            nextNumDeer++;
        else if (nextNumDeer > carryingCapacity)
            nextNumDeer--;

        if (nextNumDeer < 0)
            nextNumDeer = 0;

        WaitBarrier(); 
        NowNumDeer = nextNumDeer;
        WaitBarrier();
        WaitBarrier(); 
    }
}

void Grain() {
    while (NowYear < 2030) {
        float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.0));
        float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.0));

        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if (nextHeight < 0.) nextHeight = 0.;

        WaitBarrier(); 
        NowHeight = nextHeight;
        WaitBarrier();
        WaitBarrier(); 
    }
}

void Watcher() {
    std::ofstream outFile;
    outFile.open("output5.csv", std::ios::out | std::ios::app);

    while (NowYear < 2030) {
        WaitBarrier(); 
        WaitBarrier(); 

        if (!headerPrinted) {
            outFile << "Month,Year,Temperature,Precipitation,Grain Height,Number of Deer,Number of Komodo Dragons\n";
            headerPrinted = true;
        }

        outFile << NowMonth + 1 + NowYear * 12 << "," << NowYear << "," << NowTemp << "," 
                << NowPrecip << "," << NowHeight << "," << NowNumDeer << "," << NowNumDragons << "\n";

        NowMonth++;
        if (NowMonth > 11) {
            NowMonth = 0;
            NowYear++;
        }

        UpdateWeather();

        WaitBarrier(); 
    }

    outFile.close(); 
}

void Komodo() {
    while (NowYear < 2030) {
        int nextNumDragons = NowNumDragons;
        int expectedDeerConsumed = nextNumDragons;  

        if (expectedDeerConsumed > NowNumDeer / 2) {  
            nextNumDragons--; 
        } else if (expectedDeerConsumed < NowNumDeer / 2) {
            nextNumDragons++; 
        }

        if (nextNumDragons < 0)
            nextNumDragons = 0;

        WaitBarrier();  
        NowNumDragons = nextNumDragons;
        WaitBarrier(); 
        WaitBarrier(); 
    }
}


int main() {
    omp_set_num_threads(4);
    InitBarrier(4);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Deer();
        }
        #pragma omp section
        {
            Grain();
        }
        #pragma omp section
        {
            Watcher();
        }
        #pragma omp section
        {
            Komodo();
        }
    }

    return 0;
}


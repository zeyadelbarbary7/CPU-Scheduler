#ifndef ProcessClass_H
#define ProcessClass_H

#include <iostream>
using namespace std;

class Process{
    private:
    string name;
    int arrivalTime;
    int burstTime;
    int priority;
    int originalBurstTime;

    public:

    Process();
    Process(string name,int burst,int arrivalTime);
    Process(string name, int burst, int arrivalTime, int priority);
    void setName(string name);
    string getName();
    void setBurstTime(int burstTime);
    int getBurstTime();
    int getOriginalBurstTime();
    void setArrivalTime(int arrivalTime);
    int getArrivalTime();
    void setPriority(int priority);
    int getPriority();

};

#endif
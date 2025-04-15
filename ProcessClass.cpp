#include "ProcessClass.h"

using namespace std;

Process::Process() {
    // Default constructor body
}

Process::Process(string name, int burst, int arrivalTime) {
    this->name = name;
    this->burstTime = burst;
    this->arrivalTime = arrivalTime;
}

Process::Process(string name, int burst, int arrivalTime, int priority) {
    this->name = name;
    this->burstTime = burst;
    this->arrivalTime = arrivalTime;
    this->priority = priority;
    this->originalBurstTime = burst;
}

void Process::setName(string name) {
    this->name = name;
}

string Process::getName() {
    return this->name;
}

void Process::setBurstTime(int burstTime) {
    this->burstTime = burstTime;
}

int Process::getBurstTime() {
    return this->burstTime;
}

int Process::getOriginalBurstTime() {
    return this->originalBurstTime;
}

void Process::setArrivalTime(int arrivalTime) {
    this->arrivalTime = arrivalTime;
}

int Process::getArrivalTime() {
    return this->arrivalTime;
}

void Process::setPriority(int priority) {
    this->priority = priority;
}

int Process::getPriority() {
    return this->priority;
}
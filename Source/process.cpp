#include "process.h"
#include <string>
#include <QString>

using namespace std;

Process::Process() {}

Process::Process(QString name, int arrivalTime, int burst) {
    this->name = name.toStdString();
    this->burstTime = burst;
    this->arrivalTime = arrivalTime;
    this->originalBurstTime = burst;
    this->priority = 0;
    this->remainingTime = burst;
    this->executed = false;
    this->startTime = arrivalTime;
    this->waitingTime = 0;
    this->turnAroundTime = 0;
}

Process::Process(QString name,int arrivalTime, int burst, int priority) {
    this->name = name.toStdString();
    this->burstTime = burst;
    this->arrivalTime = arrivalTime;
    this->priority = priority;
    this->originalBurstTime = burst;
    this->priority = priority;
    this->remainingTime = burst;
    this->executed = false;
    this->startTime = arrivalTime;
    this->waitingTime = 0;
    this->turnAroundTime = 0;
}

// Setters
void Process::setName(string name)
{
    this->name = name;
}

void Process::setArrivalTime(int arrivalTime)
{
    this->arrivalTime = arrivalTime;
}

void Process::setBurstTime(int burstTime)
{
    this->burstTime = burstTime;
}

void Process::setPriority(int priority)
{
    this->priority = priority;
}

void Process::setRemainingTime(int remainingTime)
{
    this->remainingTime = remainingTime;
}

void Process::setCompletionTime(int completetionTime)
{
    this->completetionTime = completetionTime;
}

void Process::setExecuted(bool executed)
{
    this->executed = executed;
}

void Process::setStartTime(int startTime)
{
    this->startTime = startTime;
}

void Process::setTurnAroundTime(int turnAroundTime)
{
    this->turnAroundTime = turnAroundTime;
}

void Process::setWaitingTime(int waitingTime)
{
    this->waitingTime = waitingTime;
}

void Process::setId(int id)
{
    this->id = id;
}

// Getters
string Process::getName() const
{
    return name;
}

int Process::getArrivalTime() const
{
    return arrivalTime;
}

int Process::getBurstTime() const
{
    return burstTime;
}

int Process::getPriority() const
{
    return priority;
}

int Process::getRemainingTime() const
{
    return remainingTime;
}

int Process::getCompletionTime() const
{
    return completetionTime;
}

bool Process::isExecuted() const
{
    return executed;
}

int Process::getStartTime() const
{
    return startTime;
}

int Process::getTurnAroundTime() const
{
    return turnAroundTime;
}

int Process::getWaitingTime() const
{
    return waitingTime;
}

int Process::getId() const
{
    return id;
}

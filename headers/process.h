#ifndef PROCESS_H
#define PROCESS_H

#include <qcontainerfwd.h>
#include <string>
#include <QString>

using namespace std;

class Process
{
private:
    string name;
    int arrivalTime;
    int burstTime;
    int priority;
    int originalBurstTime;
    int remainingTime;
    int completetionTime;
    bool executed;
    int startTime = -1;
    int turnAroundTime;
    int waitingTime;
    int id;


public:
    Process();
    Process(QString name,int arrivalTime,int burst);
    Process(QString name, int arrivalTime, int burst, int priority);

    // Setters
    void setName(string name);
    void setArrivalTime(int arrivalTime);
    void setBurstTime(int burstTime);
    void setPriority(int priority);
    void setRemainingTime(int remainingTime);
    void setCompletionTime(int completetionTime);
    void setExecuted(bool executed);
    void setStartTime(int startTime);
    void setTurnAroundTime(int turnAroundTime);
    void setWaitingTime(int waitingTime);
    void setId(int id);

    // Getters
    string getName() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getPriority() const;
    int getRemainingTime() const;
    int getCompletionTime() const;
    bool isExecuted() const;
    int getStartTime() const;
    int getTurnAroundTime() const;
    int getWaitingTime() const;
    int getId() const;
};

#endif // PROCESS_H

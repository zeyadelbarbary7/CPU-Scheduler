#include "Priority.h"
#include <algorithm>

void priorityNonPreemptive(){
    vector <Process>processes;
    string name;
    int n,priority,burstTime,arrivalTime;

    cout<<"enter number of processes"<<endl;
    cin>>n;
    for (int i = 0; i < n; i++){
        name = "P" + to_string(i+1);
        cout<<"enter arrival time for "<<name<<endl;
        cin>>arrivalTime;
        cout<<"enter burst time for "<<name<<endl;
        cin>>burstTime;
        cout<<"enter priority for "<<name<<endl;
        cin>>priority;
        Process process = Process(name, burstTime, arrivalTime, priority);
        processes.push_back(process);
    }
    cout<<endl;
    int totalBurstTime=0;
    for (int i = 0; i < processes.size(); i++){
        totalBurstTime+=processes[i].getBurstTime();
    }
    cout<<endl;
    int time =0;
    cout<<"--- Priority Scheduling Result ---"<<endl;
    cout<<"process\tArrival\tBurst\tPriority\tstart\tend\twaiting"<<endl;
    

    vector<Process> minProcesses;
    int totalWaitingTime=0,waitingTime=0;
    do{
        for (int i = 0; i < processes.size(); ){
            if (processes[i].getArrivalTime()<=time){
                minProcesses.push_back(processes[i]);
                processes.erase(processes.begin()+i);
            }else{
                i++;
            }   
        }
        sort(minProcesses.begin(), minProcesses.end(), []( Process &p1,  Process &p2) {
            return p1.getPriority() < p2.getPriority(); // Ascending order of arrival time
        });
        cout<<minProcesses[0].getName()<<"\t"<<minProcesses[0].getArrivalTime()<<"\t"<<minProcesses[0].getBurstTime()<<"\t"<<minProcesses[0].getPriority()<<"\t\t"<<time<<"\t";
        time+=minProcesses[0].getBurstTime();
        waitingTime = time-minProcesses[0].getArrivalTime()-minProcesses[0].getBurstTime();
        cout<<time<<"\t"<<waitingTime<<endl;
        minProcesses.erase(minProcesses.begin()); // remove the process that has been executed

        totalWaitingTime+=time-minProcesses[0].getArrivalTime()-minProcesses[0].getBurstTime();
        }while (time<totalBurstTime);

        float averageWaitingTime = (float)totalWaitingTime/n;
        float averageTurnAroundTime = (float)(totalWaitingTime+totalBurstTime)/n;
        cout<<"Average Waiting Time : "<<averageWaitingTime<<endl;
        cout<<"Average Turnaround Time : "<<averageTurnAroundTime<<endl;
}

//testtttt
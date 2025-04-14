#include "Priority.h"


void priority(){
    vector <Process>processes;
    string name;
    int n,priority,burstTime,arrivalTime;

    cout<<"enter number of processes"<<endl;
    cin>>n;
    for (int i = 0; i < n; i++){
        cin>>name>>burstTime>>arrivalTime>>priority;
        Process process = Process(name, burstTime, arrivalTime, priority);
        processes.push_back(process);
        cout<<"gwa awel for";
    }
    cout<<endl;
    cout<<"abl hesab el total burst time"<<endl;
    int totalBurstTime=0;
    for (int i = 0; i < processes.size(); i++){
        totalBurstTime+=processes[i].getBurstTime();       //total processes time
        cout<<"bn7sb";
    }
    cout<<endl;
    cout<<totalBurstTime<<endl;
    int time =0;
    
    int minIndex = 0;
    int minArrival = processes[0].getArrivalTime();
    vector<Process> minProcesses;
    // minProcesses.push_back(processes[0]); 
    do{
        cout<<"gwa el do"<<endl;
        for (int i = 0; i < processes.size(); ){
            if (processes[i].getArrivalTime()<=time){
                minProcesses.push_back(processes[i]);
                processes.erase(processes.begin()+i);
                cout<<"readyProcesses";
            }else{
                i++;
            }   
        }
        cout<<endl;
        // vector<Process> minPriority;
        int higherPriorityIndex=0;
        Process currentProcess;

        if(!minProcesses.empty()){
            for (int i = 0; i < minProcesses.size(); i++){
                if (minProcesses[i].getPriority() < minProcesses[higherPriorityIndex].getPriority()){
                    higherPriorityIndex = i;
                    currentProcess = minProcesses[i];
                }
            }
            minProcesses.erase(minProcesses.begin() + higherPriorityIndex);
            time += currentProcess.getBurstTime();
            cout<<currentProcess.getName()<<endl;
            
        }
        }while (time < totalBurstTime);

    
}
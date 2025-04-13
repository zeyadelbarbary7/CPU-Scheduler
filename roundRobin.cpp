#include <iostream>
#include <utility>
#include <vector>
#include "ProcessClass.h"
using namespace std;

// void roundRobin()
int main()
{
    cout << "hello" << endl;
    int n;
    cin >> n;
    string name;
    int x, y, z;
    Process arr[5];

    for (int i = 0; i < n; i++)
    {
        cin >> name >> x >> y >> z;

        arr[i].setName(name);
        arr[i].setBurstTime(x);
        arr[i].setArrivalTime(y);
        arr[i].setPriority(z);
    }
    for (int i = 0; i < n; i++)
    {
        cout << arr[i].getName() << endl
             << arr[i].getBurstTime() << endl
             << arr[i].getArrivalTime() << endl
             << arr[i].getPriority() << endl;
    }
    return 0;
}

// int main(){

//     string type ;
//     cin >> type ;

//     vector< pair<string , int>> processes ;
// }

// cout << "Enter Number Of Processes\n" ;
// vector< pair<string , int>> processes(5) ;
// int quantumTime, numberOfProcesses ;
// cin >> numberOfProcesses ;
// for(int i=0 ; i < numberOfProcesses ; i++){
//     cout << "Enter Name of the Process:\n" ;
//     cin >> processes[i].first ;
//     cout << "Enter Burst Time of the Process:\n" ;
//     cin >> processes[i].second ;
// }
// for(int i=0 ; i < numberOfProcesses ; i++){
//     cout << processes[i].first << "\t" << processes[i].second << "\n" ;
// }
// while(!processes.empty()){

// }

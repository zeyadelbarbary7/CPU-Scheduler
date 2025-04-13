#include <iostream>
#include <utility>
#include <vector>
using namespace std;

// void roundRobin()
int main(){
    cout << "Enter Number Of Processes\n" ;
    vector< pair<string , int>> processes(5) ; 
    int quantumTime, numberOfProcesses ;
    cin >> numberOfProcesses ;
    for(int i=0 ; i < numberOfProcesses ; i++){
        cout << "Enter Name of the Process:\n" ;
        cin >> processes[i].first ;
        cout << "Enter Burst Time of the Process:\n" ;
        cin >> processes[i].second ;
    }
    for(int i=0 ; i < numberOfProcesses ; i++){
        cout << processes[i].first << "\t" << processes[i].second << "\n" ;
    }
    // while(!processes.empty()){
        
    // }
}


// int main(){

//     string type ;
//     cin >> type ;
    
//     vector< pair<string , int>> processes ;
// }
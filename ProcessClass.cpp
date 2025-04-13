#include <iostream>


using namespace std;


class Process{
  private:
    string name;
    int arrivalTime;
    int burstTime;
    int priority;

  public:

  Process(){

  }

    Process(string name,int burst,int arrivalTime){
      this->name=name;
      this->burstTime=burst;
      this->arrivalTime=arrivalTime;
    }

    Process(string name, int burst, int arrivalTime, int priority)
    {
      this->name = name;
      this->burstTime = burst;
      this->arrivalTime = arrivalTime;
      this->priority = priority;
    }

    void setName(string name){
      this->name = name;
    }
    string getName(){
      return this->name;
    }

    void setBurstTime(int burstTime)
    {
      this->burstTime = burstTime;
    }

    int getBurstTime(){
      return this->burstTime;
    }

    void setArrivalTime(int arrivalTime)
    {
      this->arrivalTime = arrivalTime;
    }

    int getArrivalTime()
    {
      return this->arrivalTime;
    }

    void setPriority(int priority)
    {
      this->priority = priority;
    }

    int getPriority()
    {
      return this->priority;
    }
};

int main(){

  //   cout<<"hello"<<endl;
  //   int n;
  //   cin>>n;
  //   string name;
  //   int x, y,z;
  //   Process arr[5];

  //   for (int i = 0; i < n; i++)
  // {
  //     cin>>name>>x>>y>>z;
    
  //     arr[i].setName(name);
  //     arr[i].setBurstTime(x);
  //     arr[i].setArrivalTime(y);
  //     arr[i].setPriority(z);
    
  // }
  //   for (int i = 0; i < n; i++)
  // {
  //     cout<<arr[i].getName()<<endl<<
  //     arr[i].getBurstTime()<<endl<<
  //     arr[i].getArrivalTime()<<endl<<
  //     arr[i].getPriority()<<endl;
  // }

  return 0;
}
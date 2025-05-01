/**
 * Elevator subsytem implementation to simulate
 * a physical elevator system. Communicates with
 * FloorSubsystem and Scheduler implementations.
 * 
 * @author Andrew Rivera
 * @version 1.00
 * January 29 3:17 Not done yet
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <iterator>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <queue>


#include "structure.hpp"

/**
 * Using data from iteration 0, assuming doorOpeningTime is roughly
 * the same as elevator unloading time, and doorOpenTime is roughly
 * the same as elevator loading time - elevator unloading time. 
 * 
 * Opening time is because the stop watch timer was started
 * as soon as the doors started to open and was stopped seconds after
 * we left the elevator without delay. 
 * 
 * Open time is assumed as such because the timer began as 
 * soon as the elevator doors started to open and stopped 
 * as soon as it completely closed. 
 * 
*/

const int doorOpeningTime = 3608; //3608ms
const int doorOpenTime = 8422 - doorOpeningTime;//4814ms
const int elevatorStoppingTime = 9030; //9030ms

const int numFloors = 10;
struct ElevatorStructure elevStruct;

class ElevatorSubsystem {
private:
    std::set<int> selectedFloors;
    int initialFloor;
    bool floorRequested = elevStruct.floorRequested;
    bool direction = elevStruct.travelDirection;

public:
    ElevatorSubsystem(const std::set<int>& a_selectedFloors, int a_initialFloor) : selectedFloors(a_selectedFloors), initialFloor(a_initialFloor), direction (elevStruct.travelDirection), floorRequested(elevStruct.floorRequested)  {}
    void operator ()(){
        //Initialize elevator buttons.
        std::set<int> buttons;
        for (int i = 1; i <= numFloors; i++){
            buttons.insert(i);
        }
        
        //Initialize elevator lamps.
        std::set<int> lamps;
        for (int i = 1; i <= numFloors; i++){
            lamps.insert(i);
        }

        //Sets current floor as inital floor
        setFloor (initialFloor); 

        bool condition = true;
        while (condition){
            monitorFloorSubsystem (selectedFloors, direction);



            std::this_thread::sleep_for( std::chrono::seconds(3) );//Testing
            condition = false;
        }
    }

    //Reads destination requests inside the elevator
    void monitorFloorSubsystem (const std::set<int>& a_selectedFloors, bool a_direction){
        if (a_selectedFloors.empty() != true){
            //Call to FloorSubsystem to retrieve floors the were pressed.
            elevStruct.floorRequested = true; 
        }
        else{
            elevStruct.floorRequested = false;
            
        }

        //Replace 0 with a call to the FloorSubsystem to retrieve the elevators destination
        int destination = 0; 
        buttonPress(elevStruct.floorRequested, destination);
    }
        

    void buttonPress (bool pressed, int dest){
        if (pressed == true){
            elevStruct.destinationFloor = dest; //Button press routed to scheduler
            setLight(dest);
        }
        else{
            elevStruct.destinationFloor = elevStruct.currentFloor;
        }
    }

    void disableLight(bool signal){
        if (elevStruct.currentFloor == elevStruct.destinationFloor){
            int destinationReached = elevStruct.currentFloor;
            elevStruct.lightOn.erase(destinationReached);
        }
    }

private:
    void setFloor(int floor){
        //std::cout<<elevStruct.currentFloor<<std::endl;
        elevStruct.currentFloor = floor;
    }

    void setLight(int floor){
        elevStruct.lightOn.insert(std::pair<int, bool>(floor, true));
    }

};

class SchedulerSystem {
private:
    std::set<int> RequestedFloors;
    std::mutex schedulerLock;
    std::queue<struct RequestStructure> requests;
    std::condition_variable_any cond;

public:
    void requestElevator(struct RequestStructure input) { // Like put, scheduler gets data from floor
        std::unique_lock<std::mutex> lock(schedulerLock);
        if(requests.size() == numFloors) cond.wait(schedulerLock);
        requests.push(input);
        cond.notify_all();
    }

    struct RequestStructure takeRequest() {
        std::unique_lock<std::mutex> lock(schedulerLock);
        if(requests.empty()) cond.wait(schedulerLock);
        struct RequestStructure output = requests.front();
        requests.pop();
        return output;
    }
};



int main()
{

    std::set<int> sampleInput = {1, 4, 2}; //Should be input file
    elevStruct.inputFile = sampleInput;

    int initialFloor = elevStruct.initialFloor;

    ElevatorSubsystem elevator(elevStruct.inputFile, initialFloor);
    std::thread elevatorThread(elevator);
    elevatorThread.join();

    return 0;

}

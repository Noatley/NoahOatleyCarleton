#if !defined(SCHEDULERSTATEMACHINE_HPP)
#define SCHEDULERSTATEMACHINE_HPP
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>

enum class SchedulerStates {LISTENING, SCHEDULING, TRANSMITTING, OPENING};

class SchedulerContext {
public:
    SchedulerContext();

    void performAction(); //Perform the current state's action.
    void setSchedulerState(SchedulerStates newState);
    int receiveRequest();       // Gets packet request
    int scheduleRequest(); //Schedule
    int transmitElevatorQueue (); //Transmits elevator queue
    const int requestQueueSize (); //Get size of request queue
    const int elevatorQueueSize(); //Get size of elevator queue
    void getRequest(std::vector<int> data); // 'Listen' and hear a packet
    void addRequestToElevator (std::vector<int> data); //Add packet to elevator's queue.
    bool getElevatorStatus (); //Determine if each component is Ready.
    bool getFloorStatus();

private:
    SchedulerStates currentSchedulerState; //Current state
    SchedulerStates prevSchedulerState; 
    std::queue<std::vector<int>> requestQueue;
    std::queue<std::vector<int>> elevatorQueue;
    std::vector<int> packet;
    bool floorReady;
    bool elevatorReady;

    void handleListening();
    void handleScheduling();
    void handleTransmitting();
    void handleOpening();
};
#endif
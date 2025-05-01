#include "SchedulerStatemachine.hpp"

SchedulerContext::SchedulerContext() : prevSchedulerState(SchedulerStates::LISTENING), currentSchedulerState(SchedulerStates::LISTENING), floorReady(false), elevatorReady(false){
    //Starts in LISTENING state, and intializes attributes to default values.
}

const int SchedulerContext::requestQueueSize() {
    return requestQueue.size();
}

const int SchedulerContext::elevatorQueueSize() {
    return elevatorQueue.size();
}

void SchedulerContext::getRequest (std::vector<int> data){
    requestQueue.push(data); //Push the packet into a queue of requested packets?
    std::cout << "Packet request added to queue" << std::endl;
}

void SchedulerContext::setSchedulerState(SchedulerStates nextState){
    prevSchedulerState = currentSchedulerState;
    currentSchedulerState = nextState;
    std::cout << "Transitioning states..." << std::endl;
}

void SchedulerContext::performAction() {
    switch (currentSchedulerState){
        case SchedulerStates::LISTENING:
            handleListening();
            break;
        case SchedulerStates::SCHEDULING:
            handleScheduling();
            break;
        case SchedulerStates::TRANSMITTING:
            handleTransmitting();
            break;

        case SchedulerStates::OPENING:
            handleOpening();
            break;
    }
}

void SchedulerContext::handleListening() {
    if(!requestQueue.empty()) { // If there are requests in the queue, move to SCHEDULING
        std::cout << "Scheduling... " << std::endl;
        SchedulerContext::setSchedulerState(SchedulerStates::SCHEDULING);
    }
}

void SchedulerContext::handleScheduling() {
    while(!requestQueue.empty()) { // Add items to elevator's queue
        std::cout << "Adding request to elevator queue." << std::endl;
        elevatorQueue.push(requestQueue.front());
        requestQueue.pop();
    }
    // Once all items are scheduled, transmit
    std::cout << "All packets scheduled. Transmitting... " << std::endl;
    SchedulerContext::setSchedulerState(SchedulerStates::TRANSMITTING);
}

void SchedulerContext::handleTransmitting() {
    std::cout << "Elevator has " << elevatorQueue.size() << " requests. Listening for ready signals." << std::endl;
    SchedulerContext::setSchedulerState(SchedulerStates::OPENING);
}

void SchedulerContext::handleOpening() { // Simulate signalling to open doors when implemented in later iterations
    while(!elevatorQueue.empty()) {
        std::cout << "Doors opening " << std::endl;
        elevatorQueue.pop();
    }
    std::cout << "All requests serviced." << std::endl;
    SchedulerContext::setSchedulerState(SchedulerStates::LISTENING);
}

int main(){
    SchedulerContext scheduler;

    //Sample input the elevator receives from the Scheduler
    std::vector<int> packetData1(5); //Empty packet of size 5
    std::vector<int> packetData2(5);

    scheduler.getRequest(packetData1);
    scheduler.getRequest(packetData2);

    while (scheduler.requestQueueSize() > 0 | scheduler.elevatorQueueSize() > 0){
        scheduler.performAction();
    }

    return 0;

}
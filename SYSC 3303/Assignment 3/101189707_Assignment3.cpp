#include "Assignment3.hpp"



Context::Context() : prevState(ContextStates::PEDESTRIANSENABLED), currentState(ContextStates::OPERATIONAL), isPedestrianWaiting(false){
    //Starts in LISTENING state, and intializes attributes to default values.
}

void Context::setState(ContextStates nextState){
    prevState = currentState;
    currentState = nextState;
    std::cout << "System: Transitioning states..." << std::endl;
}

void Context::signalPedestrians(int option) {
    if(option == DONT_WALK) {
        std::cout << "Output: DO NOT WALK" << std::endl;
    }
    else if(option == WALK) {
        std::cout << "Output: WALK" << std::endl;
    }
    else if(option == BLANK) {
        std::cout << "Output: BLANK" << std::endl;
    }
}

void Context::signalVehicles(int option) {
    switch (option){
        case GREEN:
            std::cout << "Output: GREEN LIGHT" << std::endl;
            break;
        case YELLOW:
            std::cout << "Output: YELLOW LIGHT" << std::endl;
            break;
        case RED:
            std::cout << "Output: RED LIGHT" << std::endl;
            break;        
    }
}

void Context::timeout(ContextStates state) {
    switch (state) {
        case ContextStates::VEHICLESGREEN:
            if(isPedestrianWaiting) {
                std::cout << "System: Pedestrian waiting. Stopping vehicles." << std::endl;
                Context::setState(ContextStates::VEHICLESYELLOW);
            }
            else {
                std::cout << "System: No pedestrians, timeout occurred. " << std::endl;
                Context::setState(ContextStates::VEHICLESGREENINT);
            }
            break;

        case ContextStates::VEHICLESYELLOW:
            Context::setState(ContextStates::PEDESTRIANSENABLED);
            break;

        case ContextStates::PEDESTRIANSWALK:
            Context::setState(ContextStates::PEDESTRIANSFLASH);
            break;

        case ContextStates::PEDESTRIANSFLASH:
            if(pedestrianFlashCtr == 0) {
                firstFlash = false;
                Context::setState(ContextStates::VEHICLESENABLED);
                break;
            }
            if((pedestrianFlashCtr & 1)==0){
                Context::signalPedestrians(DONT_WALK);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                Context::setState(ContextStates::PEDESTRIANSFLASH);
                break;
            }
            else {
                Context::signalPedestrians(BLANK);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                Context::setState(ContextStates::PEDESTRIANSFLASH);
                break;
            }
            break;
    }
}

void Context::setTimer(int timer) {
    for(int i = 0; i<timer; i++) {      
        std::cout << "Output: " << i+1 << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }   
}    

void Context::pedestrianWaiting() {
    std::cout << "System: Pedestrian signal" << std::endl;
    isPedestrianWaiting = true;
}

void Context::performAction() {
    switch (currentState){
        case ContextStates::OPERATIONAL:
            handleOperational();
            break;
        case ContextStates::VEHICLESENABLED:
            handleVehiclesEnabled();
            break;
        case ContextStates::VEHICLESGREEN:
            handleVehiclesGreen();
            break;
        case ContextStates::VEHICLESGREENINT:
            handleVehiclesGreenInt();
            break;
        case ContextStates::VEHICLESYELLOW:
            handleVehiclesYellow();
            break;
        case ContextStates::PEDESTRIANSENABLED:
            handlePedestriansEnabled();
            break;
        case ContextStates::PEDESTRIANSWALK:
            handlePedestriansWalk();
            break;
        case ContextStates::PEDESTRIANSFLASH:
            handlePedestriansFlash();
            break;
    }
}

void Context::handleOperational() {
    std::cout << "System: Traffic light operational. " << std::endl; 
    Context::setState(ContextStates::VEHICLESENABLED);
}

void Context::handleVehiclesEnabled() {
    signalPedestrians(DONT_WALK);
    std::cout << "System: Vehicles Enabled. " << std::endl;
    Context::setState(ContextStates::VEHICLESGREEN);
}

void Context::handleVehiclesGreen() {
    Context::signalVehicles(GREEN);
    Context::setTimer(GREENTOYELLOW);
    Context::timeout(currentState);
}

void Context::handleVehiclesGreenInt() {
    while(!isPedestrianWaiting) {}
    std::cout << "System: Pedestrian waiting. Stopping vehicles." << std::endl;
    Context::setState(ContextStates::VEHICLESYELLOW);
}

void Context::handleVehiclesYellow() {
    Context::signalVehicles(YELLOW);
    Context::setTimer(YELLOWTORED);
    Context::timeout(currentState);
}

void Context::handlePedestriansEnabled() {
    Context::signalVehicles(RED);
    Context::setState(ContextStates::PEDESTRIANSWALK);
}

void Context::handlePedestriansWalk() {
    Context::signalPedestrians(WALK);
    Context::setTimer(WALKTIMER);
    Context::timeout(currentState);
    isPedestrianWaiting = false;
}

void Context::handlePedestriansFlash() {
    if (!firstFlash) {
        firstFlash = true;
        pedestrianFlashCtr = 7;    
    }
    pedestrianFlashCtr--;
    Context::timeout(currentState);
}

void Context::runStateMachine() {
    while(true){
        performAction();
    }
}

void Context::runPedestrians() {
    int counter = 0;
    while(true){
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(counter%7 == 0) {
            pedestrianWaiting();
        }
    }
}

int main(){
    Context context;

    std::thread stateThread(&Context::runStateMachine, &context);
    std::thread pedestrianThread(&Context::runPedestrians, &context);

    stateThread.join();
    pedestrianThread.join();

    return 0;

}
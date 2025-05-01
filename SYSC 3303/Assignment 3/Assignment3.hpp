#if !defined(ASSIGNMENT3_HPP)
#define ASSIGNMENT3_HPP
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <string>
#define GREENTOYELLOW 10
#define YELLOWTORED 3
#define WALKTIMER 15
#define BLANK 0
#define DONT_WALK 1
#define WALK 2
#define GREEN 3
#define YELLOW 2
#define RED 1

enum class ContextStates {OPERATIONAL, VEHICLESENABLED, VEHICLESGREEN, VEHICLESGREENINT, VEHICLESYELLOW, PEDESTRIANSENABLED, PEDESTRIANSWALK, PEDESTRIANSFLASH};

class Context {
public:
    Context();

    void performAction(); //Perform the current state's action.
    void setState(ContextStates newState);
    void setTimer(int timer);
    void signalVehicles(int option);
    void signalPedestrians(int option);
    void timeout(ContextStates state);
    void pedestrianWaiting();
    void runStateMachine();
    void runPedestrians();

private:
    ContextStates currentState; //Current state
    ContextStates prevState;
    bool isPedestrianWaiting;
    int pedestrianFlashCtr = 7;
    bool firstFlash = false;

    void handleOperational();
    void handleVehiclesEnabled();
    void handleVehiclesGreen();
    void handleVehiclesGreenInt();
    void handleVehiclesYellow();
    void handlePedestriansEnabled();
    void handlePedestriansWalk();
    void handlePedestriansFlash();
};
#endif
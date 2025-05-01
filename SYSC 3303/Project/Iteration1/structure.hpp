#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <iostream>
#include <set>
#include <map>

struct ElevatorStructure {

    bool elevatorEnabled = false; //true if in motion, false if not in motion
    bool travelDirection = true; //true if going up, false if going down
    bool floorRequested = false; //true if button was pressed for this floor, false if there is no request
    std::map<int, bool> lightOn; //key = floor#, value = boolean true or false to indicate light
    int currentFloor = 1;
    int destinationFloor = 1;
    int initialFloor = 3;
    std::set<int>inputFile;
};

struct RequestStructure {
    int originfloor;
    std::set<int> destinations;
    bool direction;
};

#endif
Noah Oatley 101189707 Assignment 3 README

To run this program, compile Assignment3.cpp using the following command: g++ -oAssignment3 101189707_Assignment3.cpp 
then execute as follows: ./Assignment3

This compiles and runs the assignment, which creates two threads. The first thread runs the state machine infinitely, the other calls pedestrianWaiting every 7 seconds approximately.

For the two questions posed in the assignment:

1. The defect which will annoy pedestrians is that they will be forced to wait if they signal that they are waiting during the PedestriansFlash state. This was done by making isPedestrianWaiting set to false when entering the PedestriansWalk state, as well as making isPedestrianWaiting set on a separate thread, meaning the signal will be set as soon as it is called.

2. The second issue is that pedestrianFlashCtr was set to 7 every time the PedestrianFlash state was entered. This made an infinite loop as it would never relaly be decremented. This was fixed by adding a boolean for the first time entering. The boolean is initially false, and when the PedestrianFlash state is entered for the first time, it is set to true and sets the counter to 7. When the counter reaches 0 and the next state is set, it resets the boolean to false to ensure it is able to be used in future loops.
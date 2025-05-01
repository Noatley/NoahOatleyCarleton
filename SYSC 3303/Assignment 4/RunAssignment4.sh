g++ -o Host IntermediateHost.cpp 
g++ -o Client Client.cpp
g++ -o Server Server.cpp
xterm -hold -e "./Host" &
xterm -hold -e "./Client" &
xterm -hold -e "./Server" & 
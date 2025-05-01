#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Datagram.h"

#define PORT    17425
#define HOSTPORT 17401
   
class Server {
public:
    Server() : sendSocket(), receiveSocket(PORT) {}

private:
    // Construct a datagram socket and bind it to any available 
    // port on the local host machine. This socket will be used to
    // send UDP Datagram packets.
    DatagramSocket sendSocket;

    // Construct a datagram socket and bind it to port 69 
    // on the local host machine. This socket will be used to
    // receive UDP Datagram packets.
    DatagramSocket receiveSocket;

public:

void send(std::vector<uint8_t> data, int port) {
    DatagramPacket sendPacket(data, data.size(), InetAddress::getLocalHost(), port);

    std::cout << "Server: Sending packet:" << std::endl;
    std::cout << "To host: " << sendPacket.getAddressAsString() << std::endl;
    std::cout << "Destination host port: " << sendPacket.getPort() << std::endl;
    int len = sendPacket.getLength();
    std::cout << "Length: " << len << std::endl;
    std::cout << "Containing: " 
    << std::string(static_cast<const char *>(sendPacket.getData()), sendPacket.getLength()) << " in string, " 
    << sendPacket.getData() << " as bytes." << std::endl;

    // Send the datagram packet to the server via the send/receive socket. 

    try {
        sendSocket.send(sendPacket);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what();
        exit(1);
    }
}

void runServer() { 
    for(int i=0; i<11; i++) {
        std::vector<uint8_t> emptyForData;
        emptyForData.push_back(1);

        send(emptyForData, HOSTPORT);
        // Construct a DatagramPacket for receiving packets up 
        // to 100 bytes long (the length of the byte array).
        std::vector<uint8_t> receivedData(100);
        DatagramPacket receivePacket(receivedData, receivedData.size());

        try {
            // Block until a datagram is received via sendReceiveSocket.  
            receiveSocket.receive(receivePacket);
        } catch(const std::runtime_error& e) {
            std::cerr << e.what();
            exit(1);
        }

        // Process the received datagram.
        std::cout << "Client: Packet received:" << std::endl;
        std::cout << "From host: " << receivePacket.getAddressAsString() << std::endl;
        std::cout << "Host port: " << receivePacket.getPort() << std::endl;
        int len = receivePacket.getLength();
        std::cout << "Length: " << len << std::endl;
        std::cout << "Containing: "
        << std::string(static_cast<const char *>(receivePacket.getData()), receivePacket.getLength()) << " in string, " 
        << receivePacket.getData() << " as bytes." << std::endl;

        if(receivedData[1] != 9) {
            std::vector<uint8_t> sentdata;
            sentdata.push_back(0);
            sentdata.push_back(receivedData[1] == 1 ? 3 : 4); // 9 is invalid, so on the 11th send, sends 9 instead of 1 or 2
            const std::string s = "file.txt";
            sentdata.insert(sentdata.end(), s.begin(), s.end() );
            sentdata.push_back(0);
            const std::string mode = "octet";
            sentdata.insert(sentdata.end(), mode.begin(), mode.end());
            sentdata.push_back(receivedData[1] == 1 ? 1 : 0);

            send(sentdata, HOSTPORT);
        }
        else {throw std::invalid_argument("Received invalid second bit.");}
    }    
    return;
}
};
 

int main(int argc, char ** argv)
{
    Server().runServer();
}

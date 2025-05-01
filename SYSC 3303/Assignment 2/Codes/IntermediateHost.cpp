#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Datagram.h"

#define CLIENTPORT    23
#define SERVERPORT    69
   
class IntermediateHost {
public:
    IntermediateHost() : sendAndReceiveSocket(), clientSocket(CLIENTPORT) {}

private:
    // Construct a datagram socket and bind it to any available 
    // port on the local host machine. This socket will be used to
    // send UDP Datagram packets.
    DatagramSocket sendAndReceiveSocket;

    // Construct a datagram socket and bind it to port 5000 
    // on the local host machine. This socket will be used to
    // receive UDP Datagram packets.
    DatagramSocket clientSocket;

public:
    void receiveAndEcho() {

        while(true) {
        
            // Taking from client, sending to server
            std::vector<uint8_t> data(100);
            DatagramPacket receivedFromClientPacket(data, data.size(), InetAddress::getLocalHost(), CLIENTPORT);
            std::cout << "Host: Waiting for Packet from client." << std::endl;

            // Block until a datagram packet is received from receiveSocket.
            try {        
                std::cout << "Waiting..." << std::endl;
                clientSocket.receive(receivedFromClientPacket);
            } catch (const std::runtime_error& e ) {
                std::cout << "IO Exception: likely:"
                    << "Receive Socket Timed Out." << std::endl << e.what() << std::endl;
                exit(1);
            }

            // Process the received datagram.
            std::cout << "Host: Packet received from client:" << std::endl;
            std::cout << "From: " << receivedFromClientPacket.getAddressAsString() << std::endl;
            std::cout << "Host port: " << receivedFromClientPacket.getPort() << std::endl;
            int len = receivedFromClientPacket.getLength();
            std::cout << "Length: " << len << std::endl;
            std::cout << "Containing: " 
            << std::string(static_cast<const char *>(receivedFromClientPacket.getData()), receivedFromClientPacket.getLength()) << " in string, " 
            << receivedFromClientPacket.getData() << " as bytes."
            << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        
            // Creates a packet to send to the server
            DatagramPacket sendtoServerPacket(data, receivedFromClientPacket.getLength(),
                        receivedFromClientPacket.getAddress(), SERVERPORT);

            std::cout <<  "Host: Sending packet to server:" << std::endl;
            std::cout << "To host: " << sendtoServerPacket.getAddressAsString() << std::endl;
            std::cout << "Destination host port: " << sendtoServerPacket.getPort() << std::endl;
            len = sendtoServerPacket.getLength();
            std::cout << "Length: " << len << std::endl
            << "Containing: "
            << std::string(static_cast<const char *>(sendtoServerPacket.getData()), sendtoServerPacket.getLength()) << " in string, " 
            << sendtoServerPacket.getData() << " as bytes." << std::endl;
                
            // Send the datagram packet to the server via the send socket. 
            try {
                sendAndReceiveSocket.send(sendtoServerPacket);
            } catch ( const std::runtime_error& e ) {
                std::cerr << e.what() << std::endl;
                exit(1);
            }

            std::cout << "Host: packet sent" << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds( 5 ) ); // Host sent packet to server



            // Taking from Server, sending to client
            std::vector<uint8_t> data2(100);
            DatagramPacket receivedFromServerPacket(data2, data2.size(), InetAddress::getLocalHost(), SERVERPORT);
            std::cout << "Host: Waiting for Packet from server." << std::endl;

            // Block until a datagram packet is received from receiveSocket.
            try {        
                std::cout << "Waiting..." << std::endl; // so we know we're waiting
                clientSocket.receive(receivedFromServerPacket);
            } catch (const std::runtime_error& e ) {
                std::cout << "IO Exception: likely:"
                    << "Receive Socket Timed Out." << std::endl << e.what() << std::endl;
                exit(1);
            }

            // Process the received datagram.
            std::cout << "Host: Packet received from server:" << std::endl;
            std::cout << "From host: " << receivedFromServerPacket.getAddressAsString() << std::endl;
            std::cout << "Host port: " << receivedFromServerPacket.getPort() << std::endl;
            int len2 = receivedFromServerPacket.getLength();
            std::cout << "Length: " << len2 << std::endl;
            std::cout << "Containing: " 
            << std::string(static_cast<const char *>(receivedFromServerPacket.getData()), receivedFromServerPacket.getLength()) << " in string, " 
            << receivedFromServerPacket.getData() << " as bytes." << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        
            // Creates a packet to send to the server
            DatagramPacket sendtoClientPacket(data2, receivedFromServerPacket.getLength(),
                        receivedFromClientPacket.getAddress(), receivedFromClientPacket.getPort());

            std::cout <<  "Host: Sending packet to client:" << std::endl;
            std::cout << "To host: " << sendtoClientPacket.getAddressAsString() << std::endl;
            std::cout << "Destination host port: " << sendtoClientPacket.getPort() << std::endl;
            len2 = sendtoClientPacket.getLength();
            std::cout << "Length: " << len2 << std::endl
            << "Containing: "
            << std::string(static_cast<const char *>(sendtoClientPacket.getData()), sendtoClientPacket.getLength()) << " in string, " 
            << sendtoClientPacket.getData() << " as bytes." << std::endl;

                
            // Send the datagram packet to the server via the send socket. 
            try {
                sendAndReceiveSocket.send(sendtoClientPacket);
            } catch ( const std::runtime_error& e ) {
                std::cerr << e.what() << std::endl;
                exit(1);
            }

            std::cout << "Host: packet sent" << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
        }
      return;
   }
};
 

int main(int argc, char ** argv)
{
    IntermediateHost().receiveAndEcho();
}

#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Datagram.h"

#define PORT    69
#define HOSTPORT 23
   
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
    void receiveAndEcho() {

        for (int i = 0; i < 11; ++i) {

            std::vector<uint8_t> data(100);
            DatagramPacket receivePacket(data, data.size(), InetAddress::getLocalHost(), PORT);
            std::cout << "Server: Waiting for Packet." << std::endl;

            // Block until a datagram packet is received from receiveSocket.
            try {        
                std::cout << "Waiting..." << std::endl; // so we know we're waiting
                receiveSocket.receive(receivePacket);
            } catch (const std::runtime_error& e ) {
                std::cout << "IO Exception: likely:"
                    << "Receive Socket Timed Out." << std::endl << e.what() << std::endl;
                exit(1);
            }

            // Process the received datagram.
            std::cout << "Server: Packet received:" << std::endl;
            std::cout << "From host: " << receivePacket.getAddressAsString() << std::endl;
            std::cout << "Host port: " << receivePacket.getPort() << std::endl;
            int len = receivePacket.getLength();
            std::cout << "Length: " << len << std::endl;
            std::cout << "Containing: " 
            << std::string(static_cast<const char *>(receivePacket.getData()), receivePacket.getLength()) << " in string, " 
            << receivePacket.getData() << " as bytes." << std::endl;
            
            std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
        
            if (data[1] != 9) {
                // Create a new datagram packet based on the string received from the client.
            std::vector<uint8_t> sentdata;
            sentdata.push_back(0);
            sentdata.push_back(data[1] == 1 ? 3 : 4); // 9 is invalid, so on the 11th send, sends 9 instead of 1 or 2
            const std::string s = "file.txt";
            sentdata.insert(sentdata.end(), s.begin(), s.end() );
            sentdata.push_back(0);
            const std::string mode = "octet";
            sentdata.insert(sentdata.end(), mode.begin(), mode.end());
            sentdata.push_back(data[1] == 1 ? 1 : 0);

            DatagramPacket sendPacket(sentdata, receivePacket.getLength(),
                        receivePacket.getAddress(), HOSTPORT);


            std::cout <<  "Server: Sending packet:" << std::endl;
            std::cout << "To host: " << sendPacket.getAddressAsString() << std::endl;
            std::cout << "Destination host port: " << sendPacket.getPort() << std::endl;
            len = sendPacket.getLength();
            std::cout << "Length: " << len << std::endl
            << "Containing: " << std::endl;
            std::cout << std::string(static_cast<const char *>(sendPacket.getData()), len) << " in string, " 
            << receivePacket.getData() << " as bytes." << std::endl; 
            
                
            // Send the datagram packet to the client via the send socket. 
            try {
                sendSocket.send(sendPacket);
            } catch ( const std::runtime_error& e ) {
                std::cerr << e.what() << std::endl;
                exit(1);
            }

            std::cout << "Server: packet sent" << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

            // We're finished, so close the sockets.
            // Sockets will close with RAII
            //      sendSocket.close();
            //      receiveSocket.close();
            }
            else {
                throw std::invalid_argument("Received invalid second bit.");
            }
            
            }
      return;
   }
};
 

int main(int argc, char ** argv)
{
    Server().receiveAndEcho();
}

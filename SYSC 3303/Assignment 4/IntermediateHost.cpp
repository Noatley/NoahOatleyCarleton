#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Datagram.h"

#define CLIENTPORT    17430
#define SERVERPORT    17425
#define RECEIVEFROMSERVER 17401
#define RECEIVEFROMCLIENT 17402
   
class IntermediateHost {
public:
    IntermediateHost() : ClientSocket(RECEIVEFROMCLIENT), ServerSocket(RECEIVEFROMSERVER) {}

private:
    DatagramSocket sendingSocket;
    DatagramSocket ClientSocket;
    DatagramSocket ServerSocket;    

    std::vector<uint8_t> receivedFromClient;
    std::vector<uint8_t> receivedFromServer;
    std::vector<uint8_t> dataFromClient;
    std::vector<uint8_t> dataFromServer;
    std::vector<uint8_t> acknowledgement;

public:

    void reply(DatagramPacket received, int port) {
        std::vector<uint8_t> reply;
        reply.push_back(1);
        const std::string s = "Reply";
        reply.insert(reply.end(), s.begin(), s.end() );
        
        DatagramPacket replyPacket(reply, reply.size(),
                    received.getAddress(), port);

        std::cout <<  "Host: Sending Reply to client:" << std::endl;
        std::cout << "To host: " << replyPacket.getAddressAsString() << std::endl;
        std::cout << "Destination host port: " << replyPacket.getPort() << std::endl;
        int len = replyPacket.getLength();
        std::cout << "Length: " << len << std::endl
        << "Containing: "
        << std::string(static_cast<const char *>(replyPacket.getData()), replyPacket.getLength()) << std::endl;
            
        // Send the datagram packet to the server via the send socket. 
        try {
            sendingSocket.send(replyPacket);
        } catch ( const std::runtime_error& e ) {
            std::cerr << e.what() << std::endl;
            exit(1);
        }

        std::cout << "Host: packet sent" << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds( 5 ) ); // Host sent packet to server
    }

    void receiveAndAcknowledgeClient() {
        while(true) {
            // Taking from client, sending to server
            receivedFromClient.push_back(0);
            receivedFromClient.resize(20);
            DatagramPacket receivedFromClientPacket(receivedFromClient, receivedFromClient.size(), InetAddress::getLocalHost());
            std::cout << "Host: Waiting for Packet from client." << std::endl;

            // Block until a datagram packet is received from receiveSocket.
            try {        
                std::cout << "Waiting..." << std::endl;
                ClientSocket.receive(receivedFromClientPacket);
            } catch (const std::runtime_error& e ) {
                std::cout << "IO Exception: likely: "
                    << "Receive from Client Socket Timed Out." << std::endl << e.what() << std::endl;
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
        
            //If data packet has content, it is data write, if empty, it is waiting for a reply
            if(receivedFromClient[0] == 0) { // If first index is a 0, save the data and send reply
                dataFromClient = receivedFromClient;
                reply(receivedFromClientPacket, CLIENTPORT);   
            }

            else if(receivedFromClient[0]==1) { // If its a 1, it wants an acknowledgement
                DatagramPacket acknowledgementPacket(dataFromServer, dataFromServer.size(),
                            receivedFromClientPacket.getAddress(), CLIENTPORT);

                std::cout <<  "Host: Sending acknowledgement to client:" << std::endl;
                std::cout << "To host: " << acknowledgementPacket.getAddressAsString() << std::endl;
                std::cout << "Destination host port: " << acknowledgementPacket.getPort() << std::endl;
                len = acknowledgementPacket.getLength();
                std::cout << "Length: " << len << std::endl
                << "Containing: "
                << std::string(static_cast<const char *>(acknowledgementPacket.getData()), acknowledgementPacket.getLength()) << " in string, " 
                << acknowledgementPacket.getData() << " as bytes." << std::endl;
                    
                // Send the datagram packet to the server via the send socket. 
                try {
                    sendingSocket.send(acknowledgementPacket);
                } catch ( const std::runtime_error& e ) {
                    std::cerr << e.what() << std::endl;
                    exit(1);
                }

                std::cout << "Host: packet sent" << std::endl;
                std::this_thread::sleep_for( std::chrono::seconds( 5 ) ); // Host sent packet to client
            }
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
      return;
   }

   void receiveAndAcknowledgeServer() {
        while(true) {
            // Taking from Server
            receivedFromServer.push_back(0);
            receivedFromServer.resize(20);
            DatagramPacket receivedFromServerPacket(receivedFromServer, receivedFromServer.size(), InetAddress::getLocalHost());
            std::cout << "Host: Waiting for Packet from server." << std::endl;

            // Block until a datagram packet is received from receiveSocket.
            try {        
                std::cout << "Waiting..." << std::endl; // so we know we're waiting
                ServerSocket.receive(receivedFromServerPacket);
            } catch (const std::runtime_error& e ) {
                std::cout << "IO Exception: likely:"
                    << "Receive from Server Socket Timed Out." << std::endl << e.what() << std::endl;
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
        
            //If Server data is empty, it wants data, if it has content it is the acknowledgement
            if(receivedFromServer[0] == 1) {
                // Creates a packet to send to the server
                DatagramPacket replyToServerPacket(dataFromClient, dataFromClient.size(),
                receivedFromServerPacket.getAddress(), SERVERPORT);

                std::cout <<  "Host: Sending data to server:" << std::endl;
                std::cout << "To host: " << replyToServerPacket.getAddressAsString() << std::endl;
                std::cout << "Destination host port: " << replyToServerPacket.getPort() << std::endl;
                len2 = replyToServerPacket.getLength();
                std::cout << "Length: " << len2 << std::endl
                << "Containing: "
                << std::string(static_cast<const char *>(replyToServerPacket.getData()), replyToServerPacket.getLength()) << std::endl;

                    
                // Send the datagram packet to the server via the send socket. 
                try {
                    sendingSocket.send(replyToServerPacket);
                } catch ( const std::runtime_error& e ) {
                    std::cerr << e.what() << std::endl;
                    exit(1);
                }

                std::cout << "Host: packet sent" << std::endl;
                std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
            }

            // If server message has content, it has an acknowledgement in it
            else if(receivedFromServer[0] == 0) {
                //Saves data from the server to use as acknowledgement
                dataFromServer = receivedFromServer;
                reply(receivedFromServerPacket, SERVERPORT);
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
   }
};


int main(int argc, char ** argv)
{
    IntermediateHost host;
    std::thread clientThread(&IntermediateHost::receiveAndAcknowledgeClient, &host);
    std::thread serverThread(&IntermediateHost::receiveAndAcknowledgeServer, &host);

    clientThread.join();
    serverThread.join();
}

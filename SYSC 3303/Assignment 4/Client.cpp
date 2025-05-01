#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h> 
#include "Datagram.h"

#define RECEIVEPORT    17430
#define HOSTPORT    17402

class Client {
public:
    Client() : sendSocket(), receiveSocket(RECEIVEPORT) {}
    
private:
    DatagramSocket sendSocket;
    DatagramSocket receiveSocket;

public:
    void sendAndWait(std::vector<uint8_t> data) { 
        DatagramPacket sendPacket( data, data.size(), InetAddress::getLocalHost(), HOSTPORT );

        std::cout << "Client: Sending packet:" << std::endl;
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

        // Construct a DatagramPacket for receiving packets up 
        // to 100 bytes long (the length of the byte array).
        std::vector<uint8_t> in(100);
        DatagramPacket receivePacket(in, in.size());

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
        len = receivePacket.getLength();
        std::cout << "Length: " << len << std::endl;
        std::cout << "Containing: "
        << std::string(static_cast<const char *>(receivePacket.getData()), receivePacket.getLength()) << " in string, " 
        << receivePacket.getData() << " as bytes." << std::endl;
        return;
    }

    void runClient() {
        std::vector<uint8_t> emptyForAck;
        emptyForAck.push_back(1);

        for(int i=0; i<11; i++) {
            std::vector<uint8_t> out;
            out.push_back(0);
            out.push_back((i < 10) ? (i % 2 == 0 ? 1 : 2) : 9); // 9 is invalid, so on the 11th send, sends 9 instead of 1 or 2
            const std::string s = "file.txt";
            out.insert(out.end(), s.begin(), s.end() );
            out.push_back(0);
            const std::string mode = "octet";
            out.insert(out.end(), mode.begin(), mode.end());
            out.push_back(0);

            sendAndWait(out);
            sendAndWait(emptyForAck);
        }
    }
};

int main(int argc, char ** argv)
{
    Client().runClient();
}


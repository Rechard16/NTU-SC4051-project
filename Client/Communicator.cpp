#include "Communicator.h"
#include "Marshalling.h"
#include "Unmarshalling.h"
#include <iostream>
#include <WS2tcpip.h>


Communicator::Communicator(const std::string &serverIp, unsigned short serverPort): serverIp(serverIp), serverPort(serverPort){
    initializeWinsock();
    createSocket();
    setupServerAddress();
}

Communicator::~Communicator() {
    closesocket(sock);
    cleanupWinsock();
}

void Communicator::sendRequest(OperationCode operationCode, int requestID, const std::vector<int> &intParams,
                               const std::vector<std::string> &stringParams) {
    auto data = Marshalling::marshall(operationCode, requestID, intParams, stringParams);//marshall the data
    sendto(sock, reinterpret_cast<const char*>(data.data()), data.size(), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));//send the data
}
void Communicator::sendRequest(OperationCode operationCode, int requestID, const std::string &filePath, int64_t lastUpdate) {
    auto data = Marshalling::marshallUpdate(requestID, filePath, lastUpdate);//marshall the data
    sendto(sock, reinterpret_cast<const char*>(data.data()), data.size(), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));//send the data
}

bool Communicator::receiveResponse(OperationCode operationCode,int requestID, ReceivedMessage &msg,bool &repeat) {
    msg.clear();
    int serverAddrSize = sizeof(serverAddr);
    char buffer[2048];
    memset(buffer,0,sizeof(buffer));//initialize the buffer
    int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&serverAddr), &serverAddrSize);//receive the data
    //add a random number to simulate packet drop, DROP_PROB is the probability of packet drop,defined in the header file
    static std::mt19937 gen(std::random_device{}());//random number generator for drop simulation
    std::uniform_real_distribution<double> dis(0, 1);//a random number between 0 and 1 with uniform distribution
    if(dis(gen)<DROP_PROB&&bytesReceived>0){//simulate packet drop
        std::cout<<"Packet dropped"<<std::endl;
        return false;
    }
    if (bytesReceived == SOCKET_ERROR) {//if there is an error
        return false;
    }
    if(WSAGetLastError()==WSAETIMEDOUT){//if the socket times out
        return false;
    }
    if(bytesReceived == 0){//if there is no data received
        return false;
    }
    else{
        std::vector<uint8_t> data(buffer, buffer + bytesReceived);//convert the data to a vector
        Unmarshalling::unmarshall(operationCode, requestID,data, msg,repeat);//unmarshall the data
        if(repeat){//if the server send a repeat response
            return false;
        }
        return true;
    }
}
bool Communicator::sendAndReceive(OperationCode operationCode, int requestID, const std::vector<int> &intParams,
                                  const std::vector<std::string> &stringParams, ReceivedMessage &msg) {
    msg.clear();
    const int timeout = 2000;//timeout for the socket
    const int timeSleep = 500;//sleep time
    const int maxRetries = 7;//max retries
    int retries = 0;//number of retries
    bool success = false;//if the message is received successfully
    bool repeat = false;//if the server send a repeat response
    while (retries < maxRetries) {
        sendRequest(operationCode, requestID, intParams, stringParams);//send the request
        success = receiveResponse(operationCode, requestID, msg, repeat);//receive the response
        if(repeat){
            continue;
        }
        if (success) {
            break;
        }
        retries++;
        Sleep(timeSleep);
    }
    if(!success) {
        std::cerr << "Failed to send and receive message" << std::endl;
    }
    return false;
}
bool Communicator::sendAndReceive(OperationCode operationCode, int requestID, const std::string &filePath, int64_t lastUpdate, ReceivedMessage &msg) {
    msg.clear();
    const int timeout = 2000;
    const int timeSleep = 500;
    const int maxRetries = 7;
    int retries = 0;
    bool success = false;
    bool repeat = false;
    while (retries < maxRetries) {//try to send and receive the message for maxRetries times
        sendRequest(operationCode, requestID, filePath, lastUpdate);//send the request
        success = receiveResponse(operationCode, requestID, msg, repeat);//receive the response
        if(repeat){//if the server send a repeat response, it need to be filtered
            continue;
        }
        if (success) {//if the message is received successfully
            break;
        }
        retries++;//increment the retries
        Sleep(timeSleep);//sleep for a while
    }
    if(!success) {
        std::cerr << "Failed to send and receive message" << std::endl;
    }
    return false;
}

void Communicator::initializeWinsock() {
    WSADATA wsadata;
    int result = WSAStartup(MAKEWORD(2, 2), &wsadata);//initialize the winsock
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        exit(1);
    }
}

void Communicator::cleanupWinsock() {
    WSACleanup();//clean up the winsock
}

void Communicator::createSocket() {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }
    struct timeval tv;//set the timeout for the socket
    tv.tv_sec=2;//set the timeout to 2 seconds
    tv.tv_usec=0;
    if(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv))<0){
        std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl;
    }
}
bool Communicator::waitForReceiveMonitor(ReceivedMessage &msg){
    //this function is for the monitor operation
    //it will continuously receive the message from the server,and it might wait for a long time
    //so we need to set the timeout to a large value
    struct timeval tv;//set the timeout for the socket
    tv.tv_sec=2;
    tv.tv_usec=0;
    if(setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv))<0){
        std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl;
    }
    while(true){
        std::cout<<std::flush;
        int serverAddrSize = sizeof(serverAddr);
        char buffer[2048];
memset(buffer,0,sizeof(buffer));//initialize the buffer
        int bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&serverAddr), &serverAddrSize);//receive the data
        if(WSAGetLastError()==WSAETIMEDOUT){//if the socket times out
            return false;
        }
        if(bytesReceived != 0){
            std::vector<uint8_t> data(buffer, buffer + bytesReceived);//convert the data to a vector
            Unmarshalling::unmarshall(OperationCode::MONITOR, data, msg);//unmarshall the data
            return true;
        }
    }
}

void Communicator::setupServerAddress() {
    serverAddr.sin_family = AF_INET;//IPv4
    serverAddr.sin_port = htons(serverPort);//convert to network byte order
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);//convert to network byte order
}

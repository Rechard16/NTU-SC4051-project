#ifndef CLIENT_COMMUNICATOR_H
#define CLIENT_COMMUNICATOR_H

#include "Marshalling.h"
#include "ReceivedMessage.h"
#include <vector>
#include <string>
#include <WinSock2.h>
#include <random>

#define DROP_PROB 0.1
/**
 * Communicator class is responsible for sending requests to the server and receiving responses.
 * It uses the Marshalling class to convert the request to a byte array and the Unmarshalling class to convert the response to a ReceivedMessage object.
 */
class Communicator {
public:
    /**
     * Constructor
     * @param serverIp The IP address of the server
     * @param serverPort The port number of the server
     */
    Communicator(const std::string& serverIp, unsigned short serverPort);
    /**
     * Destructor
     */
    ~Communicator();
    /**
     * Sends a request to the server and waits for a response for operation which is not update
     * @param operationCode The operation code of the request
     * @param requestID The request ID
     * @param intParams The integer parameters of the request
     * @param stringParams The string parameters of the request
     * @param msg The response message
     * @return True if the response was received successfully, false otherwise
     */
    bool sendAndReceive(OperationCode operationCode, int requestID, const std::vector<int> &intParams,
                        const std::vector<std::string> &stringParams, ReceivedMessage &msg);
    /**
     * Sends a request to the server and waits for a response for update operation
     * @param operationCode The operation code of the request, actually it is always UPDATE
     * @param requestID The request ID
     * @param filePath The file path of the file to be updated
     * @param lastUpdate The last update time of the file
     * @param msg The response message
     * @return True if the response was received successfully, false otherwise
     */
    bool sendAndReceive(OperationCode operationCode, int requestID, const std::string &filePath, int64_t lastUpdate,
                        ReceivedMessage &msg);
    /**
     * Used for monitor operation, waits for a response for monitor operation
     * @param msg The response message
     * @return True if the response was received successfully, false otherwise
     */
    bool waitForReceiveMonitor(ReceivedMessage &msg);
private:
    std::string serverIp;//IP address of the server
    unsigned short serverPort;//Port number of the server
    SOCKET sock;//Socket
    struct sockaddr_in serverAddr;//Server address

    void initializeWinsock();//Initialize Winsock
    void cleanupWinsock();//Cleanup Winsock
    void createSocket();//Create a socket
    void setupServerAddress();//Setup the server address

    /**
     * Sends a request to the server for operation which is not update
     * @param operationCode The operation code of the request
     * @param requestID The request ID
     * @param intParams The integer parameters of the request
     * @param stringParams The string parameters of the request
     */
    void sendRequest(OperationCode operationCode, int requestID, const std::vector<int>& intParams, const std::vector<std::string>& stringParams);
    /**
     * Sends a request to the server for update operation
     * @param operationCode The operation code of the request, actually it is always UPDATE
     * @param requestID The request ID
     * @param filePath The file path of the file to be updated
     * @param lastUpdate The last update time of the file
     */
    void sendRequest(OperationCode operationCode, int requestID, const std::string &filePath, int64_t lastUpdate);
    /**
     * Receives a response from the server
     * @param operationCode The operation code of the request
     * @param requestID The request ID
     * @param msg The response message
     * @param repeat A flag to indicate if the request should be repeated
     * @return True if the response was received successfully, false otherwise
     */
    bool receiveResponse(OperationCode operationCode, int requestID, ReceivedMessage &msg, bool &repeat);


};


#endif //CLIENT_COMMUNICATOR_H

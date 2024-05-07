#ifndef CLIENT_RECEIVEDMESSAGE_H
#define CLIENT_RECEIVEDMESSAGE_H


#include <cstdint>
#include <vector>
#include <string>
/**
 * Class to store the response from the server
 */
class ReceivedMessage {
public:
    /**
     * Default Constructor
     */
    ReceivedMessage() = default;
    /**
     * Constructor
     * @param isSuccessful whether the request was successful
     * @param response the response from the server
     */
    ReceivedMessage(bool isSuccessful, std::string &response);
    /**
     * Constructor
     * @param isSuccessful whether the request was successful
     * @param response the response from the server
     * @param lastUpdate the last update time
     */
    ReceivedMessage(bool isSuccessful, std::string &response, int64_t lastUpdate);
    // Getters and Setters
    bool getIsSuccessful() const;
    std::string getResponse() const;
    int64_t getLastUpdate() const;
    void setLastUpdate(int64_t lastUpdate);
    void setResponse(std::string response);
    void setIsSuccessful(bool isSuccessful);
    bool getUpdateFlag() const;
    void setUpdateFlag(bool updateFlag);
    /**
     * Clear the response
     */
    void clear();
private:
    bool isSuccessful{};// whether the request was successful
    bool updateFlag{};// whether the response is an update
    std::string response;// the response from the server
    int64_t lastUpdate{};// the last update time for update operation responses
};


#endif //CLIENT_RECEIVEDMESSAGE_H

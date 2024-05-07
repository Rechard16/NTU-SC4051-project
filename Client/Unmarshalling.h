#ifndef CLIENT_UNMARSHALLING_H
#define CLIENT_UNMARSHALLING_H


#include <string>
#include <cstdint>
#include <vector>
#include "OperationCode.h"
#include "ReceivedMessage.h"
/**
 * This class is responsible for unmarshalling the data received from the server
 */
class Unmarshalling {
public:

    /**
     * Unmarshalls the data received from the server for non-monitor messages
     * @param operationCode the operation code of the message
     * @param requestId the request id of the message
     * @param data the data received from the server
     * @param message the message to be unmarshalled
     * @param repeat a flag to indicate if the message is a repeat
     */
    static void
    unmarshall(OperationCode operationCode, int requestId, const std::vector<uint8_t> &data, ReceivedMessage &message,
               bool &repeat);
    /**
     * Unmarshalls the data received from the server for monitor messages
     * @param operationCode the operation code of the message
     * @param data the data received from the server
     * @param message the message to be unmarshalled
     */
    static void
    unmarshall(OperationCode operationCode, const std::vector<uint8_t> &data, ReceivedMessage &message);
};


#endif //CLIENT_UNMARSHALLING_H

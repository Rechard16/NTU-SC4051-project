#ifndef CLIENT_MARSHALLING_H
#define CLIENT_MARSHALLING_H

#include <vector>
#include <string>
#include <cstdint>
#include "OperationCode.h"
/**
 * Class that provides methods to marshall data
 */
class Marshalling {
public:
    /**
     * Marshalls the data to be sent to the server
     * @param operationCode The operation code
     * @param requestID The request ID
     * @param intParams The integer parameters
     * @param StringParams The string parameters
     * @return The marshalled data as a vector of bytes(bytes array)
     */
    static std::vector<uint8_t> marshall(OperationCode operationCode, int requestID,
                                         const std::vector<int> &intParams, const std::vector<std::string> &StringParams);
    /**
     * Marshalls an update request
     * @param requestID The request ID
     * @param filePath The file path
     * @param lastUpdate The last update time
     * @return The marshalled data as a vector of bytes
     */
    static std::vector<uint8_t> marshallUpdate(int requestID, const std::string& filePath, int64_t lastUpdate);
    /**
     * Marshalls an integer
     * @param value The integer to be marshalled
     * @return The marshalled integer as a vector of bytes
     */
    static std::vector<uint8_t> marshallInt(int value);
    /**
     * Marshalls a string
     * @param value The string to be marshalled
     * @return The marshalled string as a vector of bytes
     */
    static std::vector<uint8_t> marshallString(const std::string& value);
    /**
     * Marshalls a long
     * @param value The long to be marshalled
     * @return The marshalled long as a vector of bytes
     */
    static std::vector<uint8_t> marshallLong(int64_t value);
};


#endif //CLIENT_MARSHALLING_H

#include "Marshalling.h"
#include <vector>
#include <string>
#include <cstdint> // for uint32_t
#include <cstring> // for memcpy
#include <WinSock2.h> // for htonl
#include <iostream>

std::vector<uint8_t> Marshalling::marshall(OperationCode operationCode, int requestID,
                                           const std::vector<int> &intParams, const std::vector<std::string> &StringParams) {
    std::vector<uint8_t> marshalledData;//vector to store marshalled data
    marshalledData.push_back(static_cast<uint8_t>(operationCode));//operation code
    auto requestIDBytes = marshallInt(requestID);//requestID
    marshalledData.insert(marshalledData.end(), requestIDBytes.begin(), requestIDBytes.end());
    for(const auto &param : intParams) {//int params
        auto paramBytes = marshallInt(param);
        marshalledData.insert(marshalledData.end(), paramBytes.begin(), paramBytes.end());
    }
       for(const auto &param : StringParams) {//string params
        auto paramBytes = marshallString(param);
        marshalledData.insert(marshalledData.end(), paramBytes.begin(), paramBytes.end());
    }
    return marshalledData;
}


std::vector<uint8_t> Marshalling::marshallInt(int value) {
    uint32_t netValue = htonl(static_cast<uint32_t>(value));//convert to network byte order
    std::vector<uint8_t> bytes(sizeof(uint32_t));//4 bytes
    std::memcpy(bytes.data(), &netValue, sizeof(uint32_t));//copy 4 bytes to bytes
    return bytes;
}
std::vector<uint8_t> Marshalling::marshallString(const std::string &value) {
    std::vector<uint8_t> marshalledData;
    auto lengthBytes = marshallInt(static_cast<int>(value.length()) ); // Length of string
    marshalledData.insert(marshalledData.end(), lengthBytes.begin(), lengthBytes.end());
    marshalledData.insert(marshalledData.end(), value.begin(), value.end());// String
    return marshalledData;
}
std::vector<uint8_t> Marshalling::marshallLong(int64_t value) {
    std::vector<uint8_t> bytes(8);
    //big endian, right shift 56, 48, 40, 32, 24, 16, 8, 0 and store in 8 bytes
    bytes[0] = (value >> 56) & 0xFF;
    bytes[1] = (value >> 48) & 0xFF;
    bytes[2] = (value >> 40) & 0xFF;
    bytes[3] = (value >> 32) & 0xFF;
    bytes[4] = (value >> 24) & 0xFF;
    bytes[5] = (value >> 16) & 0xFF;
    bytes[6] = (value >> 8) & 0xFF;
    bytes[7] = value & 0xFF;

    return bytes;
}

std::vector<uint8_t> Marshalling::marshallUpdate(int requestID, const std::string &filePath, int64_t lastUpdate) {
    std::vector<uint8_t> marshalledData;
marshalledData.push_back(static_cast<uint8_t>(OperationCode::UPDATE));
    auto requestIDBytes = marshallInt(requestID);//requestID
    marshalledData.insert(marshalledData.end(), requestIDBytes.begin(), requestIDBytes.end());
    auto lastUpdateBytes = marshallLong(lastUpdate);//lastUpdate
    marshalledData.insert(marshalledData.end(), lastUpdateBytes.begin(), lastUpdateBytes.end());
    auto filePathBytes = marshallString(filePath);//filePath
    marshalledData.insert(marshalledData.end(), filePathBytes.begin(), filePathBytes.end());
    return marshalledData;
}

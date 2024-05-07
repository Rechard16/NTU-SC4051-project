#include "ReceivedMessage.h"

ReceivedMessage::ReceivedMessage(bool isSuccessful, std::string &response) {
    this->isSuccessful = isSuccessful;
    this->response = response;
    this->lastUpdate = -1;
}

ReceivedMessage::ReceivedMessage(bool isSuccessful, std::string &response, int64_t lastUpdate) {
    this->isSuccessful = isSuccessful;
    this->response = response;
    this->lastUpdate = lastUpdate;
}

bool ReceivedMessage::getIsSuccessful() const {
    return this->isSuccessful;
}

std::string ReceivedMessage::getResponse() const{
    return this->response;
}

int64_t ReceivedMessage::getLastUpdate() const{
    return this->lastUpdate;
}

void ReceivedMessage::clear() {
    this->isSuccessful = false;
    this->response = "";
    this->lastUpdate = -1;
}

void ReceivedMessage::setLastUpdate(int64_t lastUpdate) {
    this->lastUpdate = lastUpdate;
}
void ReceivedMessage::setResponse(std::string response) {
    this->response = response;
}
void ReceivedMessage::setIsSuccessful(bool isSuccessful) {
    this->isSuccessful = isSuccessful;
}

bool ReceivedMessage::getUpdateFlag() const {
    return this->updateFlag;
}
void ReceivedMessage::setUpdateFlag(bool updateFlag) {
    this->updateFlag = updateFlag;
}

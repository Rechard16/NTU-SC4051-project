#include <iostream>
#include "Unmarshalling.h"

void Unmarshalling::unmarshall(OperationCode operationCode,int requestId, const std::vector<uint8_t>& data, ReceivedMessage &message,bool &repeat) {

    repeat = false;
    message.clear();

    int requestID = 0;//4 bytes for requestID,byte 1-4
    for(int i = 0; i < 4; i++){
        requestID = requestID << 8;
        requestID += (unsigned char)data[i+1];
    }
    if(requestID != requestId){//check whether the requestID is the same as the one currently sent
        repeat = true;
        return;
    }
    if(operationCode==OperationCode::UPDATE){//update operation

        if(data[0] == 1){//1 byte for success flag,byte 0
            message.setIsSuccessful(true);
        }
        else{//not successful, receive a error message
            message.setIsSuccessful(false);
            //4bytes for response length
            int length = 0;//4 bytes for response length,byte 5-8
            for(int i = 0; i < 4; i++){
                length = length << 8;
                length += (unsigned char)data[i+5];
            }
            //rest of the bytes are the message
            std::string response;
            for(int i = 0; i < length; i++){//read the response,start from byte 9
                response += data[i+9];
            }
            message.setResponse(response);//set the response
            return;
        }
        //4bytes for response length,byte 5-8
        int length = 0;
        for(int i = 0; i < 4; i++){
            length = length << 8;
            length += (unsigned char)data[i+5];
        }
        //1 byte for update flag,byte 9
        bool updateFlag = data[9];
        message.setUpdateFlag(updateFlag);
        if(updateFlag){//need update content
            //8 bytes for last update
            int64_t lastUpdate = 0;//8 bytes for last update,byte 10-17
            for(int i = 0; i < 8; i++){
                lastUpdate = lastUpdate << 8;
                lastUpdate += (unsigned char)data[i+10];
            }
            message.setLastUpdate(lastUpdate);//set the last update time
            //4 bytes for content length
            int contentLength = 0;//4 bytes for content length,byte 18-21
            //htonl
            for(int i = 0; i < 4; i++){
                contentLength = contentLength << 8;
                contentLength += (unsigned char)data[i+18];
            }
            //rest of the bytes are the content,start from byte 22
            std::string response;
            for(int i = 0; i < contentLength; i++){
                response += data[i+22];
            }
            message.setResponse(response);
        }
        else{//not update content,only contains a timestamp
            int64_t lastUpdate = 0;
            for(int i = 0; i < 8; i++){
                lastUpdate = lastUpdate << 8;
                lastUpdate += (unsigned char)data[i+10];
                //std::cout<<"not need update content\n"<<"time:"<<lastUpdate<<std::endl;
            }
            message.setLastUpdate(lastUpdate);
        }
        return;
    }
    //rest of the operation
    if(data[0] == 1){//1 byte for success flag,byte 0
        message.setIsSuccessful(true);
    }
    else{
        message.setIsSuccessful(false);
    }
    //The response message can be normal message or error message,but in same format
    //4bytes for response length,byte 5-8
    int length = 0;
    for(int i = 0; i < 4; i++){
        length = length << 8;
        length += (unsigned char)data[i+5];
    }
    //rest of the bytes are the message,start from byte 9
    std::string response;
    for(int i = 0; i < length; i++){
        response += data[i+9];
    }
    message.setResponse(response);
}

void
Unmarshalling::unmarshall(OperationCode operationCode, const std::vector<uint8_t> &data, ReceivedMessage &message) {
    std::cout<<"Receive monitor message"<<std::endl;
    if(data[0] == 2){//monitor flag,byte 0
        //4 bytes int 0 for check,byte 1-4, they should all be 0
        if(data[1] != 0){
            return;
        }
        if(data[2] != 0){
            return;
        }
        if(data[3] != 0){
            return;
        }
        if(data[4] != 0){
            return;
        }
        int length = 0;//4 bytes for response length,byte 5-8
        for(int i = 0; i < 4; i++){
            length = length << 8;
            length += (unsigned char)data[i+5];
        }
        //rest of the bytes are the message,start from byte 9
        std::string response;
        for(int i = 0; i < length; i++){
            response += data[i+9];
        }
        message.setResponse(response);//set the response
        return;
    }
}


/*
2
2.txt
0
aa

4
2.txt
0
2
 */
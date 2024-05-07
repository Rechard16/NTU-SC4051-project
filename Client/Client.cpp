#include "Communicator.h"
#include <iostream>
#include "OperationCode.h"
#include "CacheManager.h"
#include "ReceivedMessage.h"
#include <chrono>

#define SERVER_IP "192.168.31.61"
#define SERVER_PORT 12345
using namespace std;
/**
 * The menu function is used to display the menu of the system
 */
void menu(){
    cout<<"Please enter the operation code:"<<endl;
    cout<<"1. Read a file"<<endl;
    cout<<"2. Write a file"<<endl;
    cout<<"3. Monitor a file"<<endl;
    cout<<"4. Delete in files"<<endl;
    cout<<"5. Move a file"<<endl;
    cout<<"6. Overwrite a file"<<endl;
    cout<<"7. Exit"<<endl;
}
int requestCounter = 0;//the request ID counter, used to generate the request ID
/**
 * The main function is the entry point of the client program
 * @return
 */
int main(){
    CacheManager cacheManager;//initialize the cache manager
    cout<<"Welcome to the file remote accessing system!"<<endl;
    string serverIP;//the server IP address
    cout<<"Please enter the server IP address:"<<endl;
    cin>>serverIP;
    Communicator communicator(serverIP, SERVER_PORT);
    cout<<"Please set the freshness interval(in seconds):"<<endl;
    int freshnessInterval;//the freshness interval
    cin>>freshnessInterval;
    cacheManager.setFreshTime(freshnessInterval);//set the freshness interval
    while (true){
        menu();//display the menu
        string operationCode;
        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
        cin>>operationCode;//get the operation code
        if(operationCode == "7"){//exit code
            break;
        }
        if(operationCode != "1" && operationCode != "2" && operationCode != "3" && operationCode != "4" && operationCode != "5"&& operationCode != "6"){
            //invalid operation code process
            cout<<"Invalid operation code!"<<endl;
            cout<<"Please enter a valid operation code!"<<endl;
            continue;
        }
        cout<<"The request ID is "<<requestCounter+1<<endl;//display the request ID
        switch (operationCode[0]) {//switch the operation code
            case '1':{//read a file
                //enter the file path, offset and count
                cout<<"Please enter the file path:"<<endl;
                string filePath;
                cin>>filePath;
                int offset,count;
                cout<<"Please enter the offset(in bytes):"<<endl;
                cin>>offset;
                cout<<"Please enter the count:"<<endl;
                cin>>count;

                if(!cacheManager.contains(filePath)){//check if the file is in the cache
                    //file is not in the cache, send a request to the server to get the file
                    int requestID = ++requestCounter;
                    ReceivedMessage msg;
                    msg.clear();
                    communicator.sendAndReceive(OperationCode::UPDATE, requestID, filePath, 0, msg);
                    if(!msg.getIsSuccessful()){
                        cout<<msg.getResponse()<<endl;
                        break;//if failed to get the file, break the switch
                    }
                    cout<<"File "+filePath+" is added into cache!"<<endl;
                    cacheManager.add(filePath,msg.getLastUpdate(),msg.getResponse());
                }
                else if((!cacheManager.isFresh(filePath))){//check if the file is fresh
                    //file is in cache but not fresh, send a request to the server to get the file
                    int requestID = ++requestCounter;
                    ReceivedMessage msg;
                    msg.clear();
                    int64_t lastUpdate = cacheManager.getCacheItem(filePath).getLastUpdate();
                    communicator.sendAndReceive(OperationCode::UPDATE, requestID, filePath, lastUpdate, msg);
                    if(!msg.getIsSuccessful()){
                        cout<<msg.getResponse()<<endl;
                        break;//if failed to get the file, break the switch
                    }
                    if(msg.getUpdateFlag()){//check if the file needs to be updated
                    cacheManager.update(filePath, msg.getLastUpdate(), msg.getResponse());//update the file content and last modify time in the cache
                    }
                    else{//not updated, only update the last modify time
                        cout<<"Only the last update time is updated!"<<endl;
                        cacheManager.update(filePath, msg.getLastUpdate(), cacheManager.getData(filePath));
                    }

                }
                //get the full content of the file from the cache
                string fullContent = cacheManager.getData(filePath);
                //use the offset and count to get the content
                string content;
                if(offset>=fullContent.size()){//check if the offset is out of range
                    cout<<"Offset is out of range!"<<endl;
                    break;//if out of range, break the switch
                }
                for(int i=offset;i<offset+count && i<fullContent.size();i++){
                    content+=fullContent[i];//get the content
                }
                cout<<"Content:\n"<<content<<endl;
                break;
            }
            case '2':{//write(insert) a file
                //enter the file path, offset and content
                cout<<"Please enter the file path:"<<endl;
                string filePath;
                cin>>filePath;
                cout<<"Please enter the insertion offset:"<<endl;
                int offset;
                cin>>offset;
                cout<<"Please enter the content:"<<endl;
                string content;
                cin>>content;
                //send a request to the server to write the file
                int requestID = ++requestCounter;
                vector<int> intParams;
                vector<string> stringParams;
                //pack the int parameters
                intParams.push_back(offset);
                //pack the string parameters
                stringParams.push_back(filePath);
                stringParams.push_back(content);
                ReceivedMessage msg;
                msg.clear();//clear the message to ensure it is empty
                communicator.sendAndReceive(OperationCode::WRITE, requestID, intParams, stringParams, msg);//send the request
                //process the response
                if(msg.getIsSuccessful()){//check if the request is successful
                    cout<<msg.getResponse()<<endl;
                    if(cacheManager.contains(filePath)){//check if the file is in the cache
                        cout<<"File is erased from cache!"<<endl;
                        cacheManager.remove(filePath);//erase the file from the cache to keep consistency
                    }
                }
                else{
                    cout<<"Failed to write file!"<<endl;
                    cout<<msg.getResponse()<<endl;//display the response for error reason
                }
                intParams.clear();//clear the int parameter for next request
                stringParams.clear();//clear the string parameters for next request
                msg.clear();//clear the message for next request
                break;
            }
            case '3':{//monitor
                //enter the file path and monitoring interval
                cout<<"Please enter the file path:"<<endl;
                string filePath;
                cin>>filePath;
                cout<<"Please enter the monitoring interval(in seconds):"<<endl;
                int interval;
                cin>>interval;
                //send a request to the server to monitor the file
                int requestID = ++requestCounter;
                vector<int> intParams;
                vector<string> stringParams;
                //pack the int parameters and string parameters
                intParams.push_back(interval);
                stringParams.push_back(filePath);
                ReceivedMessage msg;
                msg.clear();
                communicator.sendAndReceive(OperationCode::MONITOR, requestID, intParams, stringParams, msg);//send the request
                if(msg.getIsSuccessful()){//check if the request is successful
                    cout<<"Monitoring is started!"<<endl;
                    //start monitoring, waiting for the server to send the update,until the time is up
                    //use a timer to monitor the time
                    auto start = chrono::high_resolution_clock::now();//start the timer
                    auto end = start + chrono::seconds(interval);//set the end time
                    while(chrono::high_resolution_clock::now()<end){//check if the time is up
                        bool mon=communicator.waitForReceiveMonitor(msg);//wait for the server to send the update
                        if (mon){//receive the update
                            cout<<"File is updated!"<<endl;
                            cout<<msg.getResponse()<<endl;
                            if(cacheManager.contains(filePath)){
                                cacheManager.remove(filePath);//remove the file from the cache to keep consistency
                            }
                        }
                        msg.clear();
                    }
                    cout<<"Monitoring is stopped!"<<endl;
                }
                else{//failed to monitor the file
                    cout<<"Failed to monitor file!"<<endl;
                    cout<<"Please try again!"<<endl;
                    msg.clear();
                    break;
                }
                intParams.clear();//clear the int parameters for next request
                stringParams.clear();//clear the string parameters for next request
                msg.clear();//clear the message for next request
                break;
            }
            case '4':{//delete
                //enter the file path, offset and count
                cout<<"Please enter the file path:"<<endl;
                string filePath;
                cin>>filePath;
                int offset,count;
                cout<<"Please enter the offset(in bytes):"<<endl;
                cin>>offset;
                cout<<"Please enter the count:"<<endl;
                cin>>count;

                int requestID = ++requestCounter;
                vector<int> intParams;
                vector<string> stringParams;
                //pack the int parameters and string parameters
                intParams.push_back(offset);
                intParams.push_back(count);
                stringParams.push_back(filePath);
                ReceivedMessage msg;
                msg.clear();
                communicator.sendAndReceive(OperationCode::DELETEB, requestID, intParams, stringParams, msg);//send the request
                if(msg.getIsSuccessful()){//check if the request is successful
                    cout<<"File is deleted successfully!"<<endl;
                    if(cacheManager.contains(filePath)){
                        cout<<"File is erased from cache!"<<endl;
                        cacheManager.remove(filePath);//remove the file from the cache to keep consistency
                    }
                }
                else{//failed to delete the file
                    cout<<"Failed to delete file!"<<endl;
                    cout<<msg.getResponse()<<endl;//print failure reason
                }
                intParams.clear();//clear the int parameters for next request
                stringParams.clear();//clear the string parameters for next request
                msg.clear();//clear the message for next request
                break;
            }
            case '5':{//move
                //enter the source file path and destination file path
                cout<<"Please enter the source file path:"<<endl;
                string sourceFilePath;
                cin>>sourceFilePath;
                cout<<"Please enter the destination file path:"<<endl;
                string destinationFilePath;
                cin>>destinationFilePath;

                int requestID = ++requestCounter;
                vector<string> stringParams;
                vector<int> intParams;//no int parameters,but need to pack as a vector
                //pack the string parameters
                stringParams.push_back(sourceFilePath);
                stringParams.push_back(destinationFilePath);
                ReceivedMessage msg;
                msg.clear();
                communicator.sendAndReceive(OperationCode::MOVE, requestID, intParams, stringParams, msg);
                if(msg.getIsSuccessful()){
                    cout<<"File is moved successfully!"<<endl;
                    if(cacheManager.contains(sourceFilePath)){
                        cout<<"Source file is erased from cache!"<<endl;
                        cacheManager.remove(sourceFilePath);//remove the source file from the cache to keep consistency
                    }
                }
                else{
                    cout<<"Failed to move file!"<<endl;
                    cout<<msg.getResponse()<<endl;//print the failure reason
                }
                stringParams.clear();
                intParams.clear();
                msg.clear();
                break;
            }
            case '6': {//overwrite
                //enter the file path, offset and content
                cout << "Please enter the file path:" << endl;
                string filePath;
                cin >> filePath;
                cout << "Please enter the overwrite offset:" << endl;
                int offset;
                cin >> offset;
                cout << "Please enter the content:" << endl;
                string content;
                cin >> content;

                int requestID = ++requestCounter;
                vector<int> intParams;
                vector<string> stringParams;
                //pack the int parameters and string parameters
                intParams.push_back(offset);
                stringParams.push_back(filePath);
                stringParams.push_back(content);
                ReceivedMessage msg;
                msg.clear();
                communicator.sendAndReceive(OperationCode::OVERWRITE, requestID, intParams, stringParams, msg);

                if (msg.getIsSuccessful()) {
                    cout << msg.getResponse() << endl;
                    if (cacheManager.contains(filePath)) {
                        cout << "File is erased from cache!" << endl;
                        cacheManager.remove(filePath);//remove the file from the cache to keep consistency
                    }
                } else {
                    cout << "Failed to overwrite file!" << endl;
                    cout << msg.getResponse() << endl;//print the failure reason
                }
                intParams.clear();//clear the int parameters for next request
                stringParams.clear();//clear the string parameters for next request
                msg.clear();//clear the message for next request
                break;
            }
            default:
                break;
        }
    }
    cout<<"Thank you for using the file remote accessing system!"<<endl;//exit message
    cout<<"Goodbye!"<<endl;
    return 0;
}

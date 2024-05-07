cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client
        Marshalling.cpp
        Marshalling.h
        OperationCode.h
        Communicator.cpp
        Communicator.h
        Unmarshalling.cpp
        Unmarshalling.h
        CacheItem.cpp
        CacheItem.h
        CacheManager.cpp
        CacheManager.h
        ReceivedMessage.cpp
        ReceivedMessage.h
        Client.cpp
        Client.cpp
        Client.cpp)
target_link_libraries(client ws2_32)

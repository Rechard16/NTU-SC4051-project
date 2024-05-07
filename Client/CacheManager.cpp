#include <chrono>
#include <stdexcept>
#include "CacheManager.h"
#include "CacheItem.h"
void CacheManager::add(const std::string &filePath,int64_t lastUpdate,const std::string &data) {
    caches[filePath]=CacheItem(filePath,lastUpdate,data);
}
void CacheManager::update(const std::string &filePath, int64_t lastUpdate, const std::string &data) {
    auto item=caches.find(filePath);
    if(item!=caches.end()){//find the file in cache
        item->second.update(lastUpdate,data);//update the file
    }else{
        caches[filePath]=CacheItem(filePath,lastUpdate,data);//add the file to cache if not found
    }
}

bool CacheManager::contains(const std::string &filePath) {
    return caches.find(filePath) != caches.end();//check if the file is in cache
}

bool CacheManager::isFresh(const std::string &filePath) {
    auto item=caches.find(filePath);//find the file in cache
    if(item!=caches.end()){//if found
        auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();//get current time
        return now-item->second.getLastUpdate()<freshTime*1000;//compare the time with freshTime
    }
    return false;//if not found, return false
}

const std::string &CacheManager::getData(const std::string &filePath) {
    auto item=caches.find(filePath);
    if(item!=caches.end()){
        return item->second.getData();//return the content of the file
    }else{
        throw std::runtime_error("No such file in cache");//throw exception if not found, but this should not happen as the file should be checked before calling this function
    }
}

CacheItem &CacheManager::getCacheItem(const std::string &filePath) {
    auto item=caches.find(filePath);
    if(item!=caches.end()){
        return item->second;//return the cache item
    }else{
        throw std::runtime_error("No such file in cache");
    }
}

void CacheManager::remove(const std::string &filePath) {
    caches.erase(filePath);//remove the file from cache
}
void CacheManager::setFreshTime(int freshTime) {
    this->freshTime=freshTime;//set the freshTime
}
int CacheManager::getFreshTime() const {
    return freshTime;
}
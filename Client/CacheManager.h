#ifndef CLIENT_CACHEMANAGER_H
#define CLIENT_CACHEMANAGER_H


#include <unordered_map>
#include "CacheItem.h"
/**
 * CacheManager is a class that manages the cache of the client.
 * It stores the cache in a map with the file path as the key and the CacheItem as the value.
 */
class CacheManager {
public:
    /**
     * Add a new file to the cache.
     * @param filePath The path of the file.
     * @param lastUpdate The last update time of the file.
     * @param data The data of the file.
     */
    void add(const std::string& filePath, int64_t lastUpdate, const std::string& data);
    /**
     * Update the file in the cache.
     * @param filePath The path of the file.
     * @param lastUpdate The last update time of the file.
     * @param data The data of the file.
     */
    void update(const std::string& filePath, int64_t lastUpdate, const std::string& data);
    /**
     * Check if the cache contains the file.
     * @param filePath The path of the file.
     * @return True if the cache contains the file, false otherwise.
     */
    bool contains(const std::string& filePath);
    /**
     * Check if the file in the cache is fresh.
     * @param filePath The path of the file.
     * @return True if the file is fresh, false otherwise.
     */
    bool isFresh(const std::string& filePath);
    //getter and setter
    const std::string& getData(const std::string& filePath);
    CacheItem& getCacheItem(const std::string& filePath);
    void setFreshTime(int freshTime);
    int getFreshTime() const;
    /**
     * Remove the file from the cache.
     * @param filePath The path of the file.
     */
    void remove(const std::string& filePath);
private:
    std::unordered_map<std::string, CacheItem> caches;//The cache map.
    int freshTime=1000;//The fresh time of the cache.
};


#endif //CLIENT_CACHEMANAGER_H

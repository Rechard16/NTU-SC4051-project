#ifndef CLIENT_CACHEITEM_H
#define CLIENT_CACHEITEM_H


#include <string>
#include <cstdint>
#include <vector>
/**
 * CacheItem is a class that represents a cache item.
 * It stores the file path, the last update time and the data of the cache item.
 */
class CacheItem {

public:
    /**
     * Default constructor
     */
    CacheItem() = default;
    /**
     * Constructor
     * @param filePath the file path of the cache item
     * @param lastUpdate the last update time of the cache item
     * @param data the data of the cache item
     */
    CacheItem(const std::string &filePath, int64_t lastUpdate, const std::string &data);
    /**
     * Update the cache item
     * @param lastUpdate the last update time of the cache item
     * @param data the data of the cache item
     */
    void update(int64_t lastUpdate, const std::string &data);
    /**
     * Get the file path of the cache item
     * @return the file path of the cache item
     */
    const std::string &getFilePath() const;
    /**
     * Get the last update time of the cache item
     * @return the last update time of the cache item
     */
    int64_t getLastUpdate() const;
    /**
     * Get the data of the cache item
     * @return the data of the cache item
     */
    const std::string &getData() const;
private:
    std::string filePath;//file path
    int64_t lastUpdate;//last update time in milliseconds
    std::string data;//content of the file


};


#endif //CLIENT_CACHEITEM_H

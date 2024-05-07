#include "CacheItem.h"

CacheItem::CacheItem(const std::string &filePath, int64_t lastUpdate, const std::string &data): filePath(filePath), lastUpdate(lastUpdate), data(data) {
}

void CacheItem::update(int64_t lastUpdate, const std::string &data) {
    this->lastUpdate = lastUpdate;
    this->data = data;

}
// Getters
const std::string &CacheItem::getFilePath() const {
    return filePath;
}

int64_t CacheItem::getLastUpdate() const {
    return lastUpdate;
}

const std::string &CacheItem::getData() const {
    return data;
}


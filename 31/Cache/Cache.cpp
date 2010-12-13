#include "Cache.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

const std::string Cache::HTTP_NOT_IMPLEMENTED("HTTP_NOT_IMPLEMENTED");
const std::string Cache::HTTP_BAD_REQUEST("HTTP_NOT_IMPLEMENTED");
const std::string Cache::HTTP_INTERNAL_ERROR("HTTP_NOT_IMPLEMENTED");

Cache *Cache::instance() {
    static Cache cache;
    return &cache;
}
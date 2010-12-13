#include "Cache.h"

const int Cache::MAX_CACHE_ENTRY_SIZE = ProxyConfig::max_cache_entry_size;
const int Cache::MAX_CACHE_SIZE = ProxyConfig::max_cache_size;

Cache *Cache::instance(){
    static Cache cache;
    return &cache;
}

Cache::Cache(){
    // adding Entries with errors
}
#include "Cache.h"

Cache *Cache::instance(){
    static Cache cache;
    return &cache;
}

Cache::Cache(){
    // adding Entries with errors
}
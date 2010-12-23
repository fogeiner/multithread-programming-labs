#include "config.h"

namespace ProxyConfig {
    const int http_default_port = 80;
    const int listening_port = 8080;
    const int backlog = 5;
    const char *http_status_400 = "HTTP/1.0 400 Bad request\r\n\r\n<html><head><title>400</title></head><body><h1>400 Bad request</h1><hr>proxy server rejected your query</body></html>";
    const char *http_status_503 = "HTTP/1.0 503 Service unavailable\r\n\r\n<html><head><title>503</title></head><body><h1>503 Service unavailable</h1><hr>proxy server couldn't connect to the requested server</body></html>";

    // 10 kB
    const size_t max_cache_entry_size = 20 * 1024 * 1024;
    // 100 Mb
    const size_t max_cache_size = 1024 * 1024 * 1024;
}

#pragma once

namespace ProxyConfig {
	const int http_default_port = 80;
	const int listening_port = 8080;
	const int backlog = 5;
	const char *ident = "Proxy";
	const char *bad_request_msg = "HTTP/1.0 400 Bad request\r\n\r\n<html><head><title>400</title></head><body><h1>400 Bad request</h1><hr>proxy server rejected your query</body></html>";
}

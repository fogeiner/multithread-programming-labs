from ProxyConnection import ProxyClient

class CacheEntry(object):
    NEW = 0
    PENDING = 1
    READY = 2
    def __init__(self, url, parsed_url, request):
        self._status = CacheEntry.NEW
        self._server = None
        self._clients = []
        self._url = url
        self._parsed_url = parsed_url
        self._request = request
        # header of the response
        self._header = ''
        # body of the response
        self._body = ''
        # temp. buffer for receving
        self._buf = ''
    def fileno(self):
        return self._server.fileno()
    def lock(self):
        pass
    def unlock(self):
        pass
    def add_client(self, client):
        client._status = ProxyClient.CACHE
        client._cache_entry = self
        self._clients.append(client)



class Cache(object):
    def __init__(self):
        self._cache = {}
    def remove_by_key(self, key):
        del self._cache[key]
    def entries(self):
       return self._cache.values()
    def get(self, key):
        return self._cache.get(key)
    def add_entry(self, url, entry):
        self._cache[url] = entry
    # here we lock cache with mutex
    def lock(self):
        pass
    def unlock(self):
        pass
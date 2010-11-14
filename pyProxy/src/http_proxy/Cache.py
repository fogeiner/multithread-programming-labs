class CacheEntry(object):
    NEW = 0
    PENDING = 1
    READY = 2
    def __init__(self, parsed_url, request):
        self._status = CacheEntry.NEW
        self._server = None
        self._clients = []
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
        self._clients.append(client)


class Cache(object):
    def __init__(self):
        self._cache = {}
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
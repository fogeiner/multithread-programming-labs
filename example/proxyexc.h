#ifndef PROXY_EXCEPTION
#define PROXY_EXCEPTION
#include<string>

class SockEx
{
    private:
    std::string message;
    
    public:
    SockEx(const std::string& mes);
    SockEx(const SockEx& old);
    SockEx();
    
    const std::string what() const;
    ~SockEx();
};

class CacheEx
{
    private:
    std::string message;
    
    public:
    CacheEx(const std::string& mes);
    CacheEx(const CacheEx& old);
    CacheEx();
    
    const std::string what() const;
    ~CacheEx();
};
#endif

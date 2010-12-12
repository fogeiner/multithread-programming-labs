#include<string>
#include<cstring>
#include<errno.h>
#include"proxyexc.h"

SockEx::SockEx(const std::string& mes)
    :message(mes)
{

}

SockEx::SockEx(const SockEx& old)
    :message(old.message)
{

}

SockEx::SockEx()
{
    const int SIZE_BUF=1024;
    char buf[SIZE_BUF];
    strerror_r(errno, buf, SIZE_BUF);
    message=std::string(buf);
}

SockEx::~SockEx()
{

}

CacheEx::CacheEx(const CacheEx& old)
    :message(old.message)
{

}

CacheEx::CacheEx()
{
    const int SIZE_BUF=1024;
    char buf[SIZE_BUF];
    strerror_r(errno, buf, SIZE_BUF);
    message=std::string(buf);
}

CacheEx::~CacheEx()
{

}

CacheEx::CacheEx(const std::string& mes)
    :message(mes)
{

}
    
const std::string SockEx::what() const
{
    return message;
}

const std::string CacheEx::what() const
{
    return message;
}

#ifndef LISTENERS
#define LISTENERS
#include"getfromcache.h"
#include<vector>

class Listeners
{
    private:
    std::vector<GetFromCache*> listeners;
    
    public:
    void add(GetFromCache* getter);
    void remove(int i);
    int getLength() const;
    
    GetFromCache* operator [] (int index);
};

#endif

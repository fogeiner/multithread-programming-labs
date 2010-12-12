#include<iostream>
#include"listeners.h"

void Listeners::add(GetFromCache* getter)
{
    listeners.push_back(getter);
    //std::cout << listeners.size() << std::endl;
}

void Listeners::remove(int i)
{
    if(i> listeners.size())
    {
	std::cerr << "Error in remove task" << std::endl;
	return;
    }
    listeners.erase(listeners.begin() + i);
}

GetFromCache* Listeners::operator [] (int index)
{
    if(index> listeners.size())
    {
	std::cerr << "Error in get task" << std::endl;
	return NULL;
    }
    return *(listeners.begin() + index);
}

int Listeners::getLength() const
{
    return listeners.size();
}

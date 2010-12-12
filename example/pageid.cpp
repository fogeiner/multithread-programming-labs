#include<iostream>
#include"pageid.h"

PageId::PageId(std::string page, int id)
{
    this->page=page;
    this->id=id;
    count=0;
}

PageId::PageId(const PageId& old)
{
    page = old.getPage();
    count = old.getCount();
    id = old.getId();
}

int PageId::getId() const
{
    return id;
}

int PageId::getCount() const
{
    return count;
}

std::string PageId::getPage() const
{
    return page;
}

void PageId::addCount()
{
    count++;
}

void PageId::editCount()
{
    count--;
}

bool PageId::operator == (const PageId& other) const
{
    return ((id==other.getId()) && (!page.compare(other.getPage())));
}

bool PageId::operator < (const PageId& other) const
{
    if(0>page.compare(other.getPage()))
    {
	return true;
    }
    if(0<page.compare(other.getPage()))
    {
	return false;
    }
    if(id < other.getId())
    {
	return true;
    } 
    return false;
}

bool PageId::operator > (const PageId& other) const
{
    return (!(*this == other) && !(*this < other));
}

PageId::~PageId()
{

};

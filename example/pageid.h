#ifndef PAGEID
#define PAGEID
#include<string>

class PageId
{
    private:
    std::string page;
    int id;
    int count;
    
    public:
    PageId(std::string page, int id);
    PageId(const PageId& old);
    std::string getPage() const;
    int getId() const;
    int getCount() const;
    void addCount();
    void editCount();
    
    bool operator == (const PageId& other) const;
    bool operator > (const PageId& other) const;
    bool operator < (const PageId& other) const;
    ~PageId();
};

#endif

#ifndef DATA_H
#define DATA_H
#include"task.h"

class Data {
private:
    void* data;
    int length;
    bool cached;
    bool ended;
    static const unsigned int limit = 1 * 1024 * 1024 * 1024;
    // 4 MB
    unsigned int count;
    tasks* task;

public:
    Data();
    Data(const void* data, int length, bool ended);
    Data(const Data& old);

    tasks* getDownloadTask() const;
    void setDownloadTask(tasks* newTask);

    const Data & operator =(const Data& other);
    bool addData(void* data, int len, bool ended);
    int getLength() const;
    void* getData() const;
    bool isEnded() const;
    bool isCached() const;
    void setCached(bool value);
    unsigned int getCount() const;
    void addCount();
    ~Data();
};

#endif

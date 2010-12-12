#ifndef PARSE_RESULT
#define PARSE_RESULT

class ParseResult
{
    public:
    std::string query;
    std::string method;
    std::string page;
    std::string protocol;
    std::string host;
    bool implement;
    
    ParseResult()
    {
    
    };
    
    ~ParseResult()
    {
    
    };
};

#endif

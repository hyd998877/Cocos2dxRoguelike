//
//  StringUtils.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#ifndef __Cocos2dRogueLike__StringUtils__
#define __Cocos2dRogueLike__StringUtils__

#include <vector>
#include <string>
#include <functional>
#include <sstream>

namespace GameCore {
    
    class StringUtils
    {
    public:
        static std::vector<std::string> split(const std::string&& s)
        {
            std::vector<std::string> res;
            size_t current = 0, found;
            while((found = s.find_first_of(',', current)) != std::string::npos){
                res.push_back(std::string(s, current, found - current));
                current = found + 1;
            }
            res.push_back(std::string(s, current, s.size() - current));
            return res;
        }
        static std::vector<int> splitInt(const std::string&& s)
        {
            std::vector<int> res;
            size_t current = 0, found;
            while((found = s.find_first_of(',', current)) != std::string::npos){
                res.push_back(atoi(std::string(s, current, found - current).c_str()));
                current = found + 1;
            }
            res.push_back(atoi(std::string(s, current, s.size() - current).c_str()));
            return res;
        }
        
        template<typename T>
        static std::string toString(T arg)
        {
            std::stringstream ss;
            ss << arg;
            return ss.str();
        }
        
        static std::string format(const char* format, ...)
        {
            #define CC_MAX_STRING_LENGTH (1024*100)
            
            std::string ret;
            
            va_list ap;
            va_start(ap, format);
            
            char* buf = (char*)malloc(CC_MAX_STRING_LENGTH);
            if (buf != nullptr)
            {
                vsnprintf(buf, CC_MAX_STRING_LENGTH, format, ap);
                ret = buf;
                free(buf);
            }
            va_end(ap);
            
            return ret;
        }
    };
}

#endif /* defined(__Cocos2dRogueLike__StringUtils__) */

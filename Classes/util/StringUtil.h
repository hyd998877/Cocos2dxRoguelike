//
//  StringUtil.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/03/02.
//
//

#ifndef __Cocos2dRogueLike__StringUtil__
#define __Cocos2dRogueLike__StringUtil__

#include <string>
#include <vector>

#include "cocos2d.h"

class StringUtil
{
public:
    static std::vector<std::string> split(std::string&& s)
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
};

#endif /* defined(__Cocos2dRogueLike__StringUtil__) */

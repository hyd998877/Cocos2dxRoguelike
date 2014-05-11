//
//  TMXGenerator.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/11.
//
//

#ifndef __Cocos2dRogueLike__TMXGenerator__
#define __Cocos2dRogueLike__TMXGenerator__

#include <iostream>

class TMXGenerator
{
public:
    static std::string generator();
private:
    TMXGenerator();
    virtual ~TMXGenerator();
};

#endif /* defined(__Cocos2dRogueLike__TMXGenerator__) */

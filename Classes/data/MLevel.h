//
//  MLevel.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#ifndef __Cocos2dRogueLike__MLevel__
#define __Cocos2dRogueLike__MLevel__

#include <iostream>

class MLevel
{
public:
    MLevel(int levelId, int exp, int growHitPoint);
    
    int getLevelId();
    int getExp();
    int getGrowHitPoint();
    
private:
    // 一意なlevelのID
    int _levelId;
    // このLevelになるために必要な累積exp
    int _exp;
    // upするパラメータ（とりあえずHP固定）
    int _growHitPoint;
};

#endif /* defined(__Cocos2dRogueLike__MLevel__) */

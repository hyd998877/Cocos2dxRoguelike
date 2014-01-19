//
//  MLevel.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#include "MLevel.h"

MLevel::MLevel(int levelId, int exp, int growHitPoint)
{
    _levelId = levelId;
    _exp = exp;
    _growHitPoint = growHitPoint;
}

int MLevel::getLevelId()
{
    return _levelId;
}

int MLevel::getExp()
{
    return _exp;
}

int MLevel::getGrowHitPoint()
{
    return _growHitPoint;
}
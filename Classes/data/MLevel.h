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
    MLevel(int levelId, int exp, int growHitPoint)
    {
        _levelId = levelId;
        _exp = exp;
        _growHitPoint = growHitPoint;
        _growAttackPoint = 1;
        _growDefencePoint = 1;
    };
    
    const int getLevelId() const { return _levelId; }
    const int getExp() const { return _exp; }
    const int getGrowHitPoint() const { return _growHitPoint; }
    const int getGrowAttackPoint() const { return _growAttackPoint; }
    const int getGrowDefencePoint() const { return _growDefencePoint; }
    
private:
    // 一意なlevelのID
    int _levelId;
    // このLevelになるために必要な累積exp
    int _exp;
    // upするパラメータ（とりあえずHP固定）
    int _growHitPoint;
    // upするパタメータ（攻撃力）
    int _growAttackPoint;
    // upするパタメータ（防御力）
    int _growDefencePoint;
};

#endif /* defined(__Cocos2dRogueLike__MLevel__) */

//
//  MLevelDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#ifndef __Cocos2dRogueLike__MLevelDao__
#define __Cocos2dRogueLike__MLevelDao__

#include <iostream>
#include <list>

namespace json11 {
    class Json;
}

class MLevel
{
public:
    MLevel()
    : _levelId(0)
    , _exp(0)
    , _growHitPoint(0)
    , _growAttackPoint(0)
    , _growDefencePoint(0)
    {
        
    }
    MLevel(int levelId, int exp, int growHitPoint,
           int growAttackPoint, int growDefencePoint)
    : _levelId(levelId)
    , _exp(exp)
    , _growHitPoint(growHitPoint)
    , _growAttackPoint(growAttackPoint)
    , _growDefencePoint(growDefencePoint)
    {
    }
    
    virtual ~MLevel()
    {
        
    }
    
    int getLevelId() const { return _levelId; }
    int getExp() const { return _exp; }
    int getGrowHitPoint() const { return _growHitPoint; }
    int getGrowAttackPoint() const { return _growAttackPoint; }
    int getGrowDefencePoint() const { return _growDefencePoint; }
    
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

class MLevelDao
{
public:
    // シングルトン
    static MLevelDao* getInstance();
    
    bool checkLevelUp(int lv, int exp);
    const MLevel selectById(int lv);
    
    void init(json11::Json json);
        
    MLevelDao();
    virtual ~MLevelDao();
private:
    std::list<MLevel> m_levelList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MLevelDao__) */

//
//  MLevelDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#include "MLevelDao.h"

#include "json11.hpp"

// シングルトン
static MLevelDao * s_m_level_dao_instance = nullptr;

MLevelDao* MLevelDao::getInstance()
{
    if (!s_m_level_dao_instance) {
        s_m_level_dao_instance = new MLevelDao();
        s_m_level_dao_instance->init();
    }
    
    return s_m_level_dao_instance;
}

MLevelDao::MLevelDao()
:m_levelList()
{
    
};

MLevelDao::~MLevelDao()
{
    s_m_level_dao_instance = nullptr;
}

void MLevelDao::init()
{
    
}

void MLevelDao::init(json11::Json json)
{
    auto item = json.array_items()[0];
    const int maxLevel = item["MAX_LEVEL"].int_value();
    const int expBase = item["EXP_BASE"].int_value();
    const int growHpCalc = item["GROW_HP_CALC"].int_value();
    const int growAtkCalc = item["GROW_ATK_CALC"].int_value();
    const int growDefCalc = item["GROW_DEF_CALC"].int_value();
    const int growAtkUpBase = item["GROW_ATK_UP_BASE"].int_value();
    const int growDefUpBase = item["GROW_DEF_UP_BASE"].int_value();
    
    // Levelマスタは計算で作る
    for (int i = 0; i < maxLevel; i++) {
        int level            = (i + 1);
        int exp              = (i + expBase) * i;
        int growHitPoint     = (growHpCalc * i);
        int growAttackPoint  = level % growAtkUpBase == 0 ? growAtkCalc : 0;
        int growDefencePoint = level % growDefUpBase == 0 ? growDefCalc : 0;
        
        m_levelList.push_back(MLevel(level,
                                     exp,
                                     growHitPoint,
                                     growAttackPoint,
                                     growDefencePoint));
    }
    printf("MLevel load completed (%d)\n", (int)json.array_items().size());
}

bool MLevelDao::checkLevelUp(int lv, int exp)
{
    auto mLevel = selectById(lv + 1);
    if (mLevel.getLevelId() != (lv + 1)) {
        // カンスト
        return false;
    }
    
    if (exp >= mLevel.getExp()) {
        return true;
    }
    return false;
}

const MLevel MLevelDao::selectById(int lv)
{
    for (auto mLevel : m_levelList) {
        if (mLevel.getLevelId() == lv) {
            return mLevel;
        }
    }
    // hitしなければ最後のlevel
    
    return MLevel();
}


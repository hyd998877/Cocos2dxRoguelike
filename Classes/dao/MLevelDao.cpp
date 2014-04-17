//
//  MLevelDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#include "MLevelDao.h"

// シングルトン
static MLevelDao * s_m_level_dao_instance = nullptr;

MLevelDao* MLevelDao::getInstance() {
    if (!s_m_level_dao_instance) {
        s_m_level_dao_instance = new MLevelDao();
        s_m_level_dao_instance->init();
    }
    
    return s_m_level_dao_instance;
}

void MLevelDao::init() {
    // TODO: (kyokomi) とりあえず手動でマスタ作成
    for (int i = 0; i < 50; i++) {
        // level 1毎に10exp必要 level1上がる毎にHpが2 * level数上がる
        MLevel level = MLevel((i + 1), ((i + 10) * i), (2 * i));
        m_levelList.push_back(level);
    }
}

bool MLevelDao::checkLevelUp(int lv, int exp) {
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

const MLevel MLevelDao::selectById(int lv) {
    for (auto mLevel : m_levelList) {
        if (mLevel.getLevelId() == lv) {
            return mLevel;
        }
    }
    // hitしなければ最後のlevel
    
    return MLevel(0,0,0);
}


MLevelDao::~MLevelDao()
{
    s_m_level_dao_instance = nullptr;
}
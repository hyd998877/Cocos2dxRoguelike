//
//  MLevelDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/19.
//
//

#include "MLevelDao.h"

// シングルトン
static MLevelDao *s_instance = nullptr;

MLevelDao* MLevelDao::getInstance()
{
    if (!s_instance)
    {
        s_instance = new MLevelDao();
        s_instance->init();
    }
    
    return s_instance;
}

void MLevelDao::init()
{
    // TODO: とりあえず手動でマスタ作成
    for (int i = 0; i < 50; i++)
    {
        // level 1毎に10exp必要 level1上がる毎にHpが3 * level数上がる
        MLevel level = MLevel(i + 1, 10 * i, 3 * (i));
        m_levelList.push_back(level);
    }
}

bool MLevelDao::checkLevelUp(int lv, int exp)
{
    auto mLevel = selectById(lv + 1);
    if (mLevel && exp >= mLevel->getExp())
    {
        return true;
    }
    return false;
}

MLevel* MLevelDao::selectById(int lv)
{
    MLevel *pRet = NULL;
    for (MLevel mLevel : m_levelList)
    {
        if (mLevel.getLevelId() == lv)
        {
            pRet = &mLevel;
            break;
        }
    }
    // hitしなければ最後のlevel
    if (!pRet)
    {
        pRet = &(*m_levelList.end());
    }
    return pRet;
}


MLevelDao::~MLevelDao()
{
    s_instance = nullptr;
}
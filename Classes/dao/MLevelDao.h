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

#include "MLevel.h"

class MLevelDao
{
public:
    // シングルトン
    static MLevelDao* getInstance();
    
    bool checkLevelUp(int lv, int exp);
    MLevel* selectById(int lv);
    
    MLevelDao()
    :m_levelList(std::list<MLevel>())
    {
        
    };
    
    ~MLevelDao();
private:
    std::list<MLevel> m_levelList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MLevelDao__) */

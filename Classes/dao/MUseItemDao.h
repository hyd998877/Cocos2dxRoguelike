//
//  MUseItemDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MUseItemDao__
#define __Cocos2dRogueLike__MUseItemDao__

#include <iostream>

#include <list>

#include "MUseItem.h"

class MUseItemDao
{
public:
    // シングルトン
    static MUseItemDao* getInstance();
    
    const MUseItem selectById(int useItemId) const;
    
    MUseItemDao()
    :m_useItemList(std::list<MUseItem>())
    {
        
    };
    
    ~MUseItemDao();
private:
    std::list<MUseItem> m_useItemList;
    
    void init();
};


#endif /* defined(__Cocos2dRogueLike__MUseItemDao__) */
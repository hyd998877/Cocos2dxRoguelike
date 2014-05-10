//
//  MAccessoryDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MAccessoryDao__
#define __Cocos2dRogueLike__MAccessoryDao__

#include <iostream>
#include <list>

#include "MAccessory.h"

class MAccessoryDao
{
public:
    // シングルトン
    static MAccessoryDao* getInstance();
    
    const MAccessory selectById(int accessoryId) const;
    
    MAccessoryDao();
    
    ~MAccessoryDao();
private:
    std::list<MAccessory> m_accessoryList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MAccessoryDao__) */

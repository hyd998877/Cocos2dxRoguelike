//
//  MWeaponDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MWeaponDao__
#define __Cocos2dRogueLike__MWeaponDao__

#include <iostream>
#include <list>

#include "MWeapon.h"

class MWeaponDao
{
public:
    // シングルトン
    static MWeaponDao* getInstance();
    
    const MWeapon selectById(int weaponId) const;
    
    MWeaponDao()
    :m_weaponList(std::list<MWeapon>())
    {
        
    };
    
    ~MWeaponDao();
private:
    std::list<MWeapon> m_weaponList;
    
    void init();
};


#endif /* defined(__Cocos2dRogueLike__MWeaponDao__) */

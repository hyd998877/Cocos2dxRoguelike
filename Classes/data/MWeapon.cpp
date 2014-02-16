//
//  MWeapon.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MWeapon.h"

MWeapon::MWeapon(int weaponId, int weaponImageId, std::string weaponName, std::string weaponDetail, int attackPoint)
{
    _weaponId = weaponId;
    _weaponImageId = weaponImageId;
    _weaponName = weaponName;
    _weaponDetail = weaponDetail;
    _attackPoint = attackPoint;
}
//
//  MWeapon.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MWeapon__
#define __Cocos2dRogueLike__MWeapon__

#include <iostream>

class MWeapon
{
public:
    MWeapon(int weaponId, int weaponImageId, std::string weaponName, std::string weaponDetail, int attackPoint)
    {
        _weaponId = weaponId;
        _weaponImageId = weaponImageId;
        _weaponName = weaponName;
        _weaponDetail = weaponDetail;
        _attackPoint = attackPoint;
    };
    
    const int getWeaponId() const {return _weaponId;}
    const int getWeaponImageId() const {return _weaponImageId;}
    const std::string getWeaponName() const {return _weaponName;}
    const std::string getWeaponDetail() const {return _weaponDetail;}
    const int getAttackPoint() const {return _attackPoint;}
    
private:
    int _weaponId;
    int _weaponImageId;
    std::string _weaponName;
    std::string _weaponDetail;
    int _attackPoint;
//    int _hitProb;
//    int _attackRangeType;
//    int _attackRangeValue;
//    int appendType;
//    int appendValue;
};


#endif /* defined(__Cocos2dRogueLike__MWeapon__) */

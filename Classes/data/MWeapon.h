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

/**
 "WEAPON_ID": "1",
 "WEAPON_IMAGE_ID": "10774",
 "WEAPON_NAME": "レイピア",
 "ATTACK_POINT": "2",
 "HIT_PROB": "100",
 "ATTACK_RANGE_TYPE": "1",
 "ATTACK_RANGE_VALUE": "1",
 "APPEND_TYPE": "NULL",
 "APPEND_VALUE": "NULL",
 */
class MWeapon
{
public:
    MWeapon(int weaponId, int weaponImageId, std::string weaponName, std::string weaponDetail, int attackPoint);
 
    const int getWeaponId() {return _weaponId;}
    const int getWeaponImageId() {return _weaponImageId;}
    const std::string getWeaponName() {return _weaponName;}
    const std::string getWeaponDetail() {return _weaponDetail;}
    const int getAttackPoint() {return _attackPoint;}
    
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

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

namespace json11 {
    class Json;
}

class MWeapon
{
public:
    MWeapon(int weaponId, int weaponImageId, std::string weaponName, std::string weaponDetail, int attackPoint)
    : _weaponId(weaponId)
    , _weaponImageId(weaponImageId)
    , _weaponName(weaponName)
    , _weaponDetail(weaponDetail)
    , _attackPoint(attackPoint)
    {
    };
    virtual ~MWeapon()
    {
        
    }
    
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

class MWeaponDao
{
public:
    // シングルトン
    static MWeaponDao* getInstance();
    
    const MWeapon& selectById(int weaponId) const;
    
    MWeaponDao();
    virtual ~MWeaponDao();
    void init(json11::Json json);
private:
    std::list<MWeapon> m_weaponList;
    
    void init();
};


#endif /* defined(__Cocos2dRogueLike__MWeaponDao__) */

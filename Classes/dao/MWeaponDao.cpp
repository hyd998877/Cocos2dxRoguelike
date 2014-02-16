//
//  MWeaponDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MWeaponDao.h"

#include "cocos2d.h"
#include "spine/Json.h"

USING_NS_CC;

// シングルトン
static MWeaponDao *s_instance = nullptr;

MWeaponDao* MWeaponDao::getInstance()
{
    if (!s_instance)
    {
        s_instance = new MWeaponDao();
        s_instance->init();
    }
    
    return s_instance;
}

void MWeaponDao::init()
{
    // Jsonを読み込む
    auto weaponMasterFile = FileUtils::getInstance()->getDataFromFile("test_master/M_WEAPON.json");
    Json* json = Json_create((char *)weaponMasterFile.getBytes());
    Json* weaponMasterJsonList = Json_getItem(Json_getItem(Json_getItem(json, "srpgquest"), "M_WEAPON"), "row");
    weaponMasterFile.clear();
    
    int weaponListSize = weaponMasterJsonList->size;
    Json* item = nullptr;
    CCLOG("weapon size = %d", weaponListSize);
    for (int i = 0; i < weaponListSize; i++)
    {
        if (item == nullptr)
        {
            item = weaponMasterJsonList->child;
        }
        else
        {
            item = item->next;
        }
        
        MWeapon weapon = MWeapon(Json_getInt(item, "WEAPON_ID", 0),
                                 Json_getInt(item, "WEAPON_IMAGE_ID", 0),
                                 Json_getString(item, "WEAPON_NAME", ""),
                                 Json_getString(item, "WEAPON_DETAIL", ""),
                                 Json_getInt(item, "ATTACK_POINT", 0));
        m_weaponList.push_back(weapon);
    }
    Json_dispose(json);
}

const MWeapon MWeaponDao::selectById(int weaponId) const
{
    for (MWeapon weapon : m_weaponList)
    {
        if (weapon.getWeaponId() == weaponId)
        {
            return weapon;
        }
    }
    return MWeapon(0, 0, "", "", 0);
}


MWeaponDao::~MWeaponDao()
{
    s_instance = nullptr;
}

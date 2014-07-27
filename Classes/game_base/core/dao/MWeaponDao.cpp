//
//  MWeaponDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MWeaponDao.h"

#include "json11.hpp"

// シングルトン
static MWeaponDao *s_m_weapon_dao_instance = nullptr;

MWeaponDao* MWeaponDao::getInstance()
{
    if (!s_m_weapon_dao_instance) {
        s_m_weapon_dao_instance = new MWeaponDao();
        s_m_weapon_dao_instance->init();
    }
    
    return s_m_weapon_dao_instance;
}

MWeaponDao::MWeaponDao()
: m_weaponList()
{
    
};

MWeaponDao::~MWeaponDao()
{
    s_m_weapon_dao_instance = nullptr;
}

void MWeaponDao::init()
{
    
}

void MWeaponDao::init(json11::Json json)
{
    for (auto &item : json.array_items()) {
        MWeapon weapon(item["WEAPON_ID"].int_value(),
                       item["WEAPON_IMAGE_ID"].int_value(),
                       item["WEAPON_NAME"].string_value(),
                       item["WEAPON_DETAIL"].string_value(),
                       item["ATTACK_POINT"].int_value());
        this->m_weaponList.push_back(weapon);
    }
    printf("MWeapon load completed (%d)\n", (int)json.array_items().size());
}

const MWeapon& MWeaponDao::selectById(int weaponId) const
{
    for (const MWeapon& weapon : m_weaponList) {
        if (weapon.getWeaponId() == weaponId) {
            return weapon;
        }
    }
    throw std::logic_error("m_weapon not found key ");
}


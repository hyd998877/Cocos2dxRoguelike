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
//    // Jsonを読み込む
//    auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/M_WEAPON.json");
//    std::string err;
//    auto json = json11::Json::parse(jsonStringFile, err);
//    if (!err.empty()) {
//        CCLOG("error = %s", err.c_str());
//    } else {
//        CCLOG("%s", json["srpgquest"].dump().c_str());
//        auto jsonArray = json["srpgquest"]["M_WEAPON"]["row"].array_items();
//        for (auto &item : jsonArray) {
//            MWeapon weapon(item["WEAPON_ID"].int_value(),
//                           item["WEAPON_IMAGE_ID"].int_value(),
//                           item["WEAPON_NAME"].string_value(),
//                           item["WEAPON_DETAIL"].string_value(),
//                           item["ATTACK_POINT"].int_value());
//            this->m_weaponList.push_back(weapon);
//        }
//    }
}

void MWeaponDao::init(json11::Json json)
{
    // Jsonを読み込む
    for (auto &item : json.array_items()) {
        MWeapon weapon(item["WEAPON_ID"].int_value(),
                       item["WEAPON_IMAGE_ID"].int_value(),
                       item["WEAPON_NAME"].string_value(),
                       item["WEAPON_DETAIL"].string_value(),
                       item["ATTACK_POINT"].int_value());
        this->m_weaponList.push_back(weapon);
    }
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


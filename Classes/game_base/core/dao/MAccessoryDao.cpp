//
//  MAccessoryDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MAccessoryDao.h"

#include "json11.hpp"

// シングルトン
static MAccessoryDao *s_m_accessory_dao_instance = nullptr;

MAccessoryDao* MAccessoryDao::getInstance()
{
    if (!s_m_accessory_dao_instance) {
        s_m_accessory_dao_instance = new MAccessoryDao();
        s_m_accessory_dao_instance->init();
    }
    
    return s_m_accessory_dao_instance;
}

MAccessoryDao::MAccessoryDao()
: m_accessoryList()
{
    
};

MAccessoryDao::~MAccessoryDao()
{
    s_m_accessory_dao_instance = nullptr;
}

void MAccessoryDao::init()
{
//    // Jsonを読み込む
//    auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/M_ACCESSORY.json");
//    
//    std::string err;
//    auto json = json11::Json::parse(jsonStringFile, err);
//    if (!err.empty()) {
//        CCLOG("error = %s", err.c_str());
//    } else {
//        CCLOG("%s", json["srpgquest"].dump().c_str());
//        
//        auto jsonArray = json["srpgquest"]["M_ACCESSORY"]["row"].array_items();
//        for (auto &item : jsonArray) {
//            MAccessory mAccessory(item["ACCESSORY_ID"].int_value(),
//                                  item["ACCESSORY_IMAGE_ID"].int_value(),
//                                  item["ACCESSORY_NAME"].string_value(),
//                                  item["ACCESSORY_DETAIL"].string_value(),
//                                  item["DEFENSE_POINT"].int_value());
//            this->m_accessoryList.push_back(mAccessory);
//        }
//    }
}

void MAccessoryDao::init(json11::Json json)
{
    for (auto &item : json.array_items()) {
        MAccessory mAccessory(item["ACCESSORY_ID"].int_value(),
                              item["ACCESSORY_IMAGE_ID"].int_value(),
                              item["ACCESSORY_NAME"].string_value(),
                              item["ACCESSORY_DETAIL"].string_value(),
                              item["DEFENSE_POINT"].int_value());
        this->m_accessoryList.push_back(mAccessory);
    }
}

const MAccessory& MAccessoryDao::selectById(int accessoryId) const
{
    for (const MAccessory& accessory : m_accessoryList) {
        if (accessory.getAccessoryId() == accessoryId) {
            return accessory;
        }
    }
    throw std::logic_error("m_accessory not found key ");
}


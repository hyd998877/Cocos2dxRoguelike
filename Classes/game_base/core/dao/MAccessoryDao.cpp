//
//  MAccessoryDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MAccessoryDao.h"

#include "cocos2d.h"
#include "json11.hpp"

USING_NS_CC;

MAccessoryDao::MAccessoryDao()
: m_accessoryList()
{
    
};

// シングルトン
static MAccessoryDao *s_m_accessory_dao_instance = nullptr;

MAccessoryDao* MAccessoryDao::getInstance()
{
    if (!s_m_accessory_dao_instance)
    {
        s_m_accessory_dao_instance = new MAccessoryDao();
        s_m_accessory_dao_instance->init();
    }
    
    return s_m_accessory_dao_instance;
}

void MAccessoryDao::init()
{
    // Jsonを読み込む
    auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/M_ACCESSORY.json");
    
    std::string err;
    auto json = json11::Json::parse(jsonStringFile, err);
    if (!err.empty()) {
        CCLOG("error = %s", err.c_str());
    } else {
        CCLOG("%s", json["srpgquest"].dump().c_str());
        
        auto jsonArray = json["srpgquest"]["M_ACCESSORY"]["row"].array_items();
        
        for (auto &item : jsonArray) {
            MAccessory mAccessory(item["ACCESSORY_ID"].int_value(),
                                  item["ACCESSORY_IMAGE_ID"].int_value(),
                                  item["ACCESSORY_NAME"].string_value(),
                                  item["ACCESSORY_DETAIL"].string_value(),
                                  item["DEFENSE_POINT"].int_value());
            this->m_accessoryList.push_back(mAccessory);
        }
    }
}

const MAccessory MAccessoryDao::selectById(int accessoryId) const
{
    for (MAccessory accessory : m_accessoryList)
    {
        if (accessory.getAccessoryId() == accessoryId)
        {
            return accessory;
        }
    }
    return MAccessory(0, 0, "", "", 0);
}


MAccessoryDao::~MAccessoryDao()
{
    s_m_accessory_dao_instance = nullptr;
}

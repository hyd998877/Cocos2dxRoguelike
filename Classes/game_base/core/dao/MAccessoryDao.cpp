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
    printf("MAccessory load completed (%d)\n", (int)json.array_items().size());
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


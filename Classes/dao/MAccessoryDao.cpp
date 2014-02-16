//
//  MAccessoryDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MAccessoryDao.h"

#include "cocos2d.h"
#include "spine/Json.h"

USING_NS_CC;

// シングルトン
static MAccessoryDao *s_instance = nullptr;

MAccessoryDao* MAccessoryDao::getInstance()
{
    if (!s_instance)
    {
        s_instance = new MAccessoryDao();
        s_instance->init();
    }
    
    return s_instance;
}

void MAccessoryDao::init()
{
    // Jsonを読み込む
    auto accessoryMasterFile = FileUtils::getInstance()->getDataFromFile("test_master/M_ACCESSORY.json");
    Json* json = Json_create((char *)accessoryMasterFile.getBytes());
    Json* accessoryMasterJsonList = Json_getItem(Json_getItem(Json_getItem(json, "srpgquest"), "M_ACCESSORY"), "row");
    accessoryMasterFile.clear();
    
    int accessoryListSize = accessoryMasterJsonList->size;
    Json* item = nullptr;
    CCLOG("accessory size = %d", accessoryListSize);
    for (int i = 0; i < accessoryListSize; i++)
    {
        if (item == nullptr)
        {
            item = accessoryMasterJsonList->child;
        }
        else
        {
            item = item->next;
        }
        
        MAccessory accessory = MAccessory(Json_getInt(item, "ACCESSORY_ID", 0),
                                    Json_getInt(item, "ACCESSORY_IMAGE_ID", 0),
                                    Json_getString(item, "ACCESSORY_NAME", ""),
                                    Json_getString(item, "ACCESSORY_DETAIL", ""),
                                    Json_getInt(item, "DEFENSE_POINT", 0));
        m_accessoryList.push_back(accessory);
    }
    Json_dispose(json);
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
    s_instance = nullptr;
}

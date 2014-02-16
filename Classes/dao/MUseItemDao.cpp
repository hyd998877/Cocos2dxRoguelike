//
//  MUseItemDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MUseItemDao.h"

#include "cocos2d.h"
#include "spine/Json.h"

USING_NS_CC;

// シングルトン
static MUseItemDao *s_instance = nullptr;

MUseItemDao* MUseItemDao::getInstance()
{
    if (!s_instance)
    {
        s_instance = new MUseItemDao();
        s_instance->init();
    }
    
    return s_instance;
}

void MUseItemDao::init()
{
    // Jsonを読み込む
    auto useItemMasterFile = FileUtils::getInstance()->getDataFromFile("test_master/M_USE_ITEM.json");
    Json* json = Json_create((char *)useItemMasterFile.getBytes());
    Json* useItemMasterJsonList = Json_getItem(Json_getItem(Json_getItem(json, "srpgquest"), "M_USE_ITEM"), "row");
    useItemMasterFile.clear();
    
    int useItemListSize = useItemMasterJsonList->size;
    Json* item = nullptr;
    CCLOG("useItem size = %d", useItemListSize);
    for (int i = 0; i < useItemListSize; i++)
    {
        if (item == nullptr)
        {
            item = useItemMasterJsonList->child;
        }
        else
        {
            item = item->next;
        }
        
        MUseItem::ItemType itemType = static_cast<MUseItem::ItemType>(Json_getInt(item, "USE_ITEM_TYPE", 0));
        
        MUseItem useItem = MUseItem(Json_getInt(item, "USE_ITEM_ID", 0),
                                    Json_getInt(item, "USE_ITEM_IMAGE_ID", 0),
                                    itemType,
                                    Json_getString(item, "USE_ITEM_NAME", ""),
                                    Json_getString(item, "USE_ITEM_DETAIL", ""),
                                    Json_getInt(item, "USE_ITEM_PARAM", 0));
        m_useItemList.push_back(useItem);
    }
    Json_dispose(json);
}

const MUseItem MUseItemDao::selectById(int useItemId) const
{
    for (MUseItem useItem : m_useItemList)
    {
        if (useItem.getUseItemId() == useItemId)
        {
            return useItem;
        }
    }
    return MUseItem(0, 0, MUseItem::ItemType::NONE, "", "", 0);
}


MUseItemDao::~MUseItemDao()
{
    s_instance = nullptr;
}

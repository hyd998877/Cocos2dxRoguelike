//
//  MUseItemDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MUseItemDao.h"

#include "json11.hpp"

// シングルトン
static MUseItemDao *s_m_user_item_dao_instance = nullptr;

MUseItemDao* MUseItemDao::getInstance()
{
    if (!s_m_user_item_dao_instance) {
        s_m_user_item_dao_instance = new MUseItemDao();
        s_m_user_item_dao_instance->init();
    }
    
    return s_m_user_item_dao_instance;
}

MUseItemDao::MUseItemDao()
: m_useItemList()
{
};

MUseItemDao::~MUseItemDao()
{
    s_m_user_item_dao_instance = nullptr;
}

void MUseItemDao::init()
{
//    // Jsonを読み込む
//    auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/M_USE_ITEM.json");
//    std::string err;
//    auto json = json11::Json::parse(jsonStringFile, err);
//    if (!err.empty()) {
//        CCLOG("error = %s", err.c_str());
//    } else {
//        CCLOG("%s", json["srpgquest"].dump().c_str());
//        auto jsonArray = json["srpgquest"]["M_USE_ITEM"]["row"].array_items();
//        
//        for (auto &item : jsonArray) {
//            MUseItem mUseItem(item["USE_ITEM_ID"].int_value(),
//                              item["USE_ITEM_IMAGE_ID"].int_value(),
//                              static_cast<MUseItem::ItemType>(item["USE_ITEM_TYPE"].int_value()),
//                              item["USE_ITEM_NAME"].string_value(),
//                              item["USE_ITEM_DETAIL"].string_value(),
//                              item["USE_ITEM_PARAM"].int_value());
//            this->m_useItemList.push_back(mUseItem);
//        }
//    }
}

void MUseItemDao::init(json11::Json json)
{
    // Jsonを読み込む
    for (auto &item : json.array_items()) {
        MUseItem mUseItem(item["USE_ITEM_ID"].int_value(),
                          item["USE_ITEM_IMAGE_ID"].int_value(),
                          static_cast<MUseItem::ItemType>(item["USE_ITEM_TYPE"].int_value()),
                          item["USE_ITEM_NAME"].string_value(),
                          item["USE_ITEM_DETAIL"].string_value(),
                          item["USE_ITEM_PARAM"].int_value());
        this->m_useItemList.push_back(mUseItem);
    }
}

const MUseItem & MUseItemDao::selectById(int useItemId) const
{
    auto it = std::find_if(m_useItemList.cbegin(), m_useItemList.cend(), [useItemId](const MUseItem& mUseItem) -> bool {
        if (mUseItem.getUseItemId() == useItemId) {
            return true;
        }
        return false;
    });
    return *(it);
}


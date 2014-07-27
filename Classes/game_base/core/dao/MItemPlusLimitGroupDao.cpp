//
//  MItemPlusLimitGroup.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#include "MItemPlusLimitGroupDao.h"

#include "json11.hpp"

// シングルトン
static MItemPlusLimitGroupDao *s_mItemPlusLimitGroupDao_instance = nullptr;

MItemPlusLimitGroupDao* MItemPlusLimitGroupDao::getInstance()
{
    if (!s_mItemPlusLimitGroupDao_instance) {
        s_mItemPlusLimitGroupDao_instance = new MItemPlusLimitGroupDao();
        s_mItemPlusLimitGroupDao_instance->init();
    }
    
    return s_mItemPlusLimitGroupDao_instance;
}

MItemPlusLimitGroupDao::MItemPlusLimitGroupDao()
: _mItemPlusLimitGroupList()
{
    
};

MItemPlusLimitGroupDao::~MItemPlusLimitGroupDao()
{
    s_mItemPlusLimitGroupDao_instance = nullptr;
}

void MItemPlusLimitGroupDao::init()
{
    
}

void MItemPlusLimitGroupDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        MItemPlusLimitGroup data;
        data.itemPlusLimitGroupId = item["ITEM_PLUS_LIMIT_GROUP_ID"].int_value();
        data.plusValue = item["PLUS_VALUE"].int_value();
        data.prob = item["PROB"].int_value();
        
        _mItemPlusLimitGroupList.push_back(data);
    }
    printf("MItemPlusLimitGroup load completed (%d)\n", (int)json.array_items().size());
}

std::list<MItemPlusLimitGroup> MItemPlusLimitGroupDao::selectById(int itemPlusLimitGroupId) const
{
    std::list<MItemPlusLimitGroup> list;
    for (const MItemPlusLimitGroup& itemPlusGroup : _mItemPlusLimitGroupList) {
        if (itemPlusGroup.itemPlusLimitGroupId == itemPlusLimitGroupId) {
            list.push_back(itemPlusGroup);
        }
    }
    return list;
}

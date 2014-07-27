//
//  MDropGroup.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#include "MDropGroup.h"

#include "json11.hpp"

// シングルトン
static MDropGroupDao *s_mDropGroupDao_instance = nullptr;

MDropGroupDao* MDropGroupDao::getInstance()
{
    if (!s_mDropGroupDao_instance) {
        s_mDropGroupDao_instance = new MDropGroupDao();
        s_mDropGroupDao_instance->init();
    }
    
    return s_mDropGroupDao_instance;
}

MDropGroupDao::MDropGroupDao()
: _mDropGroupList()
{
    
};

MDropGroupDao::~MDropGroupDao()
{
    s_mDropGroupDao_instance = nullptr;
}

void MDropGroupDao::init()
{
    
}

void MDropGroupDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        MDropGroup data;
        data.dropGroupId = item["DROP_GROUP_ID"].int_value();
        data.dropItemType = item["DROP_ITEM_TYPE"].int_value();
        data.dropItemId = item["DROP_ITEM_ID"].int_value();
        data.prob = item["PROB"].int_value();
        
        _mDropGroupList.push_back(data);
    }
    printf("MDropGroup load completed (%d)\n", (int)json.array_items().size());
}

const MDropGroup& MDropGroupDao::selectById(int dropGroupId) const
{
    for (const MDropGroup& dropGroup : _mDropGroupList) {
        if (dropGroup.dropGroupId == dropGroupId) {
            return dropGroup;
        }
    }
    throw std::logic_error("m_drop_group not found key ");
}

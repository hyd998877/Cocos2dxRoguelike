//
//  MMobGroup.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#include "MMobGroup.h"

#include "json11.hpp"

// シングルトン
static MMobGroupDao *s_mMobGroupDao_instance = nullptr;

MMobGroupDao* MMobGroupDao::getInstance()
{
    if (!s_mMobGroupDao_instance) {
        s_mMobGroupDao_instance = new MMobGroupDao();
        s_mMobGroupDao_instance->init();
    }
    
    return s_mMobGroupDao_instance;
}

MMobGroupDao::MMobGroupDao()
: _mMobGroupList()
{
    
};

MMobGroupDao::~MMobGroupDao()
{
    s_mMobGroupDao_instance = nullptr;
}

void MMobGroupDao::init()
{
    
}

void MMobGroupDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        MMobGroup data;
        data.mobGroupId = item["MOB_GROUP_ID"].int_value();
        data.monsterId = item["MONSTER_ID"].int_value();
        data.popProb = item["POP_PROB"].int_value();
        data.dropProb = item["DROP_PROB"].int_value();
        
        _mMobGroupList.push_back(data);
    }
    printf("MMobGroup load completed (%d)\n", (int)json.array_items().size());
}

const MMobGroup& MMobGroupDao::selectById(int mobGroupId) const
{
    for (const MMobGroup& mobGroup : _mMobGroupList) {
        if (mobGroup.mobGroupId == mobGroupId) {
            return mobGroup;
        }
    }
    throw std::logic_error("m_mob_group not found key ");
}

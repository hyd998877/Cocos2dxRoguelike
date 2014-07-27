//
//  MQuestFloor.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#include "MQuestFloorDao.h"

#include "json11.hpp"

// シングルトン
static MQuestFloorDao *s_m_quest_floor_dao_instance = nullptr;

MQuestFloorDao* MQuestFloorDao::getInstance()
{
    if (!s_m_quest_floor_dao_instance) {
        s_m_quest_floor_dao_instance = new MQuestFloorDao();
        s_m_quest_floor_dao_instance->init();
    }
    
    return s_m_quest_floor_dao_instance;
}

MQuestFloorDao::MQuestFloorDao()
: _mQuestFloorList()
{
    
};

MQuestFloorDao::~MQuestFloorDao()
{
    s_m_quest_floor_dao_instance = nullptr;
}

void MQuestFloorDao::init()
{
    
}

void MQuestFloorDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        MQuestFloor data;
        data.floorId = item["FLOOR_ID"].int_value();
        data.mobCount = item["MOB_COUNT"].int_value();
        data.mobGroupId = item["MOB_GROUP_ID"].int_value();
        data.dropCount = item["DROP_COUNT"].int_value();
        data.dropGroupId = item["DROP_GRPUP_ID"].int_value();
        data.goldMin = item["GOLD_MIN"].int_value();
        data.goldMax = item["GOLD_MAX"].int_value();
        data.itemPlusLimitGroupId = item["ITEM_PLUS_LIMIT_GROUP_ID"].int_value();
        
        _mQuestFloorList.push_back(data);
    }
    printf("MQuestFloor load completed (%d)\n", (int)json.array_items().size());
}

const MQuestFloor& MQuestFloorDao::selectById(int floorId) const
{
    for (const MQuestFloor& floor : _mQuestFloorList) {
        if (floor.floorId == floorId) {
            return floor;
        }
    }
    throw std::logic_error("m_quest_floor not found key ");
}

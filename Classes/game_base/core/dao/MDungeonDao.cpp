//
//  MDungeonDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#include "MDungeonDao.h"

#include "StringUtils.h"

#include "json11.hpp"

// シングルトン
static MDungeonDao *s_m_dungeon_dao_instance = nullptr;

MDungeonDao* MDungeonDao::getInstance()
{
    if (!s_m_dungeon_dao_instance) {
        s_m_dungeon_dao_instance = new MDungeonDao();
        s_m_dungeon_dao_instance->init();
    }
    
    return s_m_dungeon_dao_instance;
}

MDungeonDao::MDungeonDao()
: _MDungeonList()
{
    
};

MDungeonDao::~MDungeonDao()
{
    s_m_dungeon_dao_instance = nullptr;
}

void MDungeonDao::init()
{
    
}

void MDungeonDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        MDungeon dungeon;
        dungeon.dungeonKey = item["DUNGEON_KEY"].string_value();
        dungeon.dungeonName = item["DUNGEON_NAME"].string_value();
        auto floors = item["FLOORS"].string_value();
        dungeon.floorIds = GameCore::StringUtils::splitInt(std::string(floors));
        dungeon.clearCount = item["CLEAR_COUNT"].int_value();
        
        _MDungeonList.push_back(dungeon);
    }
    printf("MDungeon load completed (%d)\n", (int)json.array_items().size());
}

const MDungeon& MDungeonDao::selectById(const std::string& dungeonKey) const
{
    for (const MDungeon& dungeon : _MDungeonList) {
        if (dungeon.dungeonKey == dungeonKey) {
            return dungeon;
        }
    }
    throw std::logic_error("m_dungeon not found key");
}

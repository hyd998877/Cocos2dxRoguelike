//
//  MPlayerDao.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/21.
//
//

#include "MPlayerDao.h"

#include "json11.hpp"

#include "ActorDto.h"

// シングルトン
static MPlayerDao *s_m_monster_dao_instance = nullptr;

MPlayerDao* MPlayerDao::getInstance()
{
    if (!s_m_monster_dao_instance) {
        s_m_monster_dao_instance = new MPlayerDao();
        s_m_monster_dao_instance->init();
    }
    
    return s_m_monster_dao_instance;
}

MPlayerDao::MPlayerDao()
: _mMonsterList()
{
    
};

MPlayerDao::~MPlayerDao()
{
    s_m_monster_dao_instance = nullptr;
}

void MPlayerDao::init()
{
    
}

void MPlayerDao::init(json11::Json json)
{
    for (auto item : json.array_items()) {
        ActorDto actorDto(item["playerId"].int_value(),
                          item["imageResId"].int_value(),
                          item["faceImgId"].int_value(),
                          item["name"].string_value(),
                          item["attackPoint"].int_value(),
                          item["defencePoint"].int_value(),
                          item["lv"].int_value(),
                          item["exp"].int_value(),
                          item["nextExp"].int_value(),
                          item["hitPoint"].int_value(),
                          item["hitPointLimit"].int_value(),
                          item["magicPoint"].int_value(),
                          item["magicPointLimit"].int_value(),
                          item["movePoint"].int_value(),
                          item["attackRange"].int_value(),
                          ActorEquipDto(),
                          ActorEquipDto(),
                          item["gold"].int_value());
        this->_mMonsterList.push_back(actorDto);
    }
}

const ActorDto& MPlayerDao::selectById(int monsterId) const
{
    for (const ActorDto& monster : _mMonsterList) {
        if (monster.getPlayerId() == monsterId) {
            return monster;
        }
    }
    throw std::logic_error("m_monster not found key ");
}
//
//  AccountData.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "AccountData.h"
#include "StringUtil.h"

static AccountData *s_account_data_instance;

// シングルトン
AccountData* AccountData::getInstance()
{
    CCLOG("%s %d getInstance ", __FILE__, __LINE__);
    if (!s_account_data_instance)
    {
        CCLOG("%s %d new ", __FILE__, __LINE__);
        s_account_data_instance = new AccountData();
        s_account_data_instance->init();
    }
    
    return s_account_data_instance;
}

void AccountData::init()
{
    load();
}

#pragma mark
#pragma mark static method

void AccountData::save()
{
    CCLOG("%s %d save ", __FILE__, __LINE__);
    
    UserDefault::getInstance()->setStringForKey("rogue_play_data", RogueScene::roguePlayDataToString(rogue_play_data_));
    
    UserDefault::getInstance()->setStringForKey("player_actor", ActorSprite::actorToString(player_actor_));
    UserDefault::getInstance()->setStringForKey("player_actor.equip", ActorSprite::actorEquipToString(player_actor_.equip));
    
    // 書き込み
    UserDefault::getInstance()->flush();
}

void AccountData::load()
{
    CCLOG("%s %d load ", __FILE__, __LINE__);

    std::string rogue_play_data_str = UserDefault::getInstance()->getStringForKey("rogue_play_data");
    if (rogue_play_data_str.size() > 0)
    {
        rogue_play_data_ = RogueScene::createRoguePlayData(rogue_play_data_str);
    }
    
    std::string player_actor_data_str = UserDefault::getInstance()->getStringForKey("player_actor");
    if (player_actor_data_str.size() > 0)
    {
        player_actor_ = ActorSprite::createActorDto(player_actor_data_str);
    }
    std::string player_actor_equip_data_str = UserDefault::getInstance()->getStringForKey("player_actor.equip");
    if (player_actor_equip_data_str.size() > 0)
    {
        player_actor_.equip = ActorSprite::createEquipDto(player_actor_equip_data_str);
    }
}

void AccountData::reset()
{
    CCLOG("%s %d reset ", __FILE__, __LINE__);
    
    UserDefault::getInstance()->setStringForKey("rogue_play_data", "");
    
    UserDefault::getInstance()->setStringForKey("player_actor", "");
    UserDefault::getInstance()->setStringForKey("player_actor.equip", "");
    
    UserDefault::getInstance()->flush();
}



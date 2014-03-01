//
//  AccountData.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "AccountData.h"

#include "cocos2d.h"

static AccountData *s_account_data_instance;

// シングルトン
AccountData* AccountData::getInstance()
{
    if (!s_account_data_instance)
    {
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
    // --- ローグプレイ情報 ---
    cocos2d::Data rogue_play_data;
    int rogue_play_data_size = sizeof(RogueScene::RoguePlayData);
    CCLOG("rogue_play_data_size = %d", rogue_play_data_size);
    rogue_play_data.copy((unsigned char* )&rogue_play_data_, rogue_play_data_size);
    UserDefault::getInstance()->setDataForKey("rogue_play_data", rogue_play_data);
    rogue_play_data.clear();
    
    // --- プレイヤー情報 ---
    cocos2d::Data player_actor_data;
    int actor_dto_size = sizeof(ActorSprite::ActorDto);
    CCLOG("actor_dto_size = %d", actor_dto_size);
    player_actor_data.copy((unsigned char* )&player_actor_, actor_dto_size);
    UserDefault::getInstance()->setDataForKey("player_actor", player_actor_data);
    player_actor_data.clear();
    
    // --- インベントリ ---
    // 所持品、装備状態
    int item_count = 0;
    int item_size = item_list_.size();
    if (item_size > 0)
    {
        for (DropItemSprite::DropItemDto dropItem : item_list_)
        {
            cocos2d::Data item_list_data;
            item_list_data.fastSet((unsigned char* )&dropItem, sizeof(dropItem));
            UserDefault::getInstance()->setDataForKey(StringUtils::format("item_list.item%d", item_count).c_str(), item_list_data);
            item_list_data.clear();

            item_count++;
        }
    }
    UserDefault::getInstance()->setIntegerForKey("item_list_count", item_count);
    
    // 書き込み
    UserDefault::getInstance()->flush();
}

void AccountData::load()
{
    // --- ローグプレイ情報 ---
    cocos2d::Data rogue_play_data = UserDefault::getInstance()->getDataForKey("rogue_play_data");
    if (!rogue_play_data.isNull())
    {
        RogueScene::RoguePlayData* rogue_play = static_cast<RogueScene::RoguePlayData*>((void *)rogue_play_data.getBytes());
        rogue_play_data_ = *(rogue_play);
        rogue_play_data.clear();
    }
    
    // --- プレイヤー情報 ---
    cocos2d::Data player_actor_data = UserDefault::getInstance()->getDataForKey("player_actor");
    if (!player_actor_data.isNull())
    {
        ActorSprite::ActorDto* player_actor = static_cast<ActorSprite::ActorDto*>((void *)player_actor_data.getBytes());
        player_actor_ = *(player_actor);
        
        player_actor_data.clear();
    }
    
    // --- アイテム情報 ---
    
    int item_list_count = UserDefault::getInstance()->getIntegerForKey("item_list_count", 0);
    for (int i = 0; i < item_list_count; i++)
    {
        cocos2d::Data item_data = UserDefault::getInstance()->getDataForKey(StringUtils::format("item_list.item%d", i).c_str());
        if (!item_data.isNull())
        {
            DropItemSprite::DropItemDto* drop_item = static_cast<DropItemSprite::DropItemDto*>((void *)item_data.getBytes());
            item_list_.push_back(*drop_item);
            
            item_data.clear();
        }
    }
}

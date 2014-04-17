//
//  AccountData.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "AccountData.h"

NS_ROGUE_BEGIN

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
    CCLOG("%s %d save ", __FILE__, __LINE__);
    
    ValueVector save_item_list;
    
    auto it = item_list_.begin();
    for (int i = 0; i < USE_ITEM_MAX; i++)
    {
        if (it != item_list_.end())
        {
            auto item_str = DropItemSprite::dropItemToString(*(it));
            it++;
            
            save_item_list.push_back(Value(item_str));
        }
        else
        {
            // 空データ保存
            save_item_list.push_back(Value(""));
        }
    }
    
    ValueMap save_data{
        {"rogue_play_data", Value(RogueScene::roguePlayDataToString(rogue_play_data_))},
        {"player_actor", Value(ActorSprite::actorToString(player_actor_))},
        {"player_actor.equip", Value(ActorSprite::actorEquipToString(player_actor_.equip))},
        {"item_list", Value(save_item_list)}
    };
    
    // 書き込み
    FileUtils::getInstance()->writeToFile(save_data, FileUtils::getInstance()->getWritablePath() + "save_data.plist");
}

void AccountData::load()
{
    std::string save_file_path = FileUtils::getInstance()->getWritablePath() + "save_data.plist";
    
    CCLOG("%s %d load %s ", __FILE__, __LINE__, save_file_path.c_str());
    
    ValueMap save_data = FileUtils::getInstance()->getValueMapFromFile(save_file_path);
    if (save_data.empty())
    {
        return;
    }
    Value rogue_play_data_value = save_data.at("rogue_play_data");
    std::string rogue_play_data_str = rogue_play_data_value.asString();
    if (rogue_play_data_str.size() > 0)
    {
        rogue_play_data_ = RogueScene::createRoguePlayData(rogue_play_data_str);
    }

    Value player_actor_data_value = save_data.at("player_actor");
    std::string player_actor_data_str = player_actor_data_value.asString();
    if (player_actor_data_str.size() > 0)
    {
        player_actor_ = ActorSprite::createActorDto(player_actor_data_str);
    }
    
    Value player_actor_equip_data_value = save_data.at("player_actor.equip");
    std::string player_actor_equip_data_str = player_actor_equip_data_value.asString();
    if (player_actor_equip_data_str.size() > 0)
    {
        player_actor_.equip = ActorSprite::createEquipDto(player_actor_equip_data_str);
    }
    
    ValueVector item_list_value = save_data.at("item_list").asValueVector();
    item_list_.clear();
    for (Value item_value : item_list_value)
    {
        std::string item_data_str = item_value.asString();
        if (item_data_str.size() > 0)
        {
            auto item = DropItemSprite::createDropItemDto(item_data_str);
            item_list_.push_back(item);
        }
    }
}

void AccountData::reset()
{
    CCLOG("%s %d reset ", __FILE__, __LINE__);
    
    std::string save_file_path = FileUtils::getInstance()->getWritablePath() + "save_data.plist";
    remove(save_file_path.c_str());
    
    rogue_play_data_ = RogueScene::createRoguePlayData("");
    player_actor_ = ActorSprite::createDto();
    item_list_.clear();
}

NS_ROGUE_END

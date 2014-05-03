//
//  AccountData.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "AccountData.h"

#include "MRogueMap.h"

NS_ROGUE_BEGIN

static std::string createSaveFilePath();
static void remoteSaveFile();
static void clearRoguePlayData();
static void clearPlayerActorData();
static void clearItemList();

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
    for (int i = 0; i < m_rogue_map::USE_ITEM_MAX; i++)
    {
        if (it != item_list_.end())
        {
            auto item_str = (*(it)).itemDtoToString();
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
        {"player_actor", Value(player_actor_.actorToString())},
        {"player_actor.weapon_equip", Value(player_actor_.getWeaponEquip().actorEquipToString())},
        {"player_actor.accessory_equip", Value(player_actor_.getAccessoryEquip().actorEquipToString())},
        {"item_list", Value(save_item_list)}
    };
    
    // 書き込み
    FileUtils::getInstance()->writeToFile(save_data, createSaveFilePath());
}

void AccountData::load()
{
    std::string save_file_path = createSaveFilePath();
    CCLOG("%s %d load %s ", __FILE__, __LINE__, save_file_path.c_str());
    
    ValueMap save_data = FileUtils::getInstance()->getValueMapFromFile(save_file_path);
    if (save_data.empty()) {
        return;
    }
    
    Value rogue_play_data_value = save_data.at("rogue_play_data");
    std::string rogue_play_data_str = rogue_play_data_value.asString();
    if (rogue_play_data_str.size() > 0) {
        rogue_play_data_ = RogueScene::createRoguePlayData(rogue_play_data_str);
    }

    Value player_actor_data_value = save_data.at("player_actor");
    std::string player_actor_data_str = player_actor_data_value.asString();
    if (player_actor_data_str.size() > 0) {
        player_actor_ = ActorDto::createActorDto(player_actor_data_str);
    }
    
    Value player_actor_weapon_equip_data_value = save_data.at("player_actor.weapon_equip");
    std::string player_actor_weapon_equip_data_str = player_actor_weapon_equip_data_value.asString();
    if (player_actor_weapon_equip_data_str.size() > 0) {
        player_actor_.setWeaponEquip(ActorEquipDto::createEquipDto(player_actor_weapon_equip_data_str));
    }
    
    Value player_actor_accessory_equip_data_value = save_data.at("player_actor.accessory_equip");
    std::string player_actor_accessory_equip_data_str = player_actor_accessory_equip_data_value.asString();
    if (player_actor_accessory_equip_data_str.size() > 0) {
        player_actor_.setAccessoryEquip(ActorEquipDto::createEquipDto(player_actor_accessory_equip_data_str));
    }
    
    ValueVector item_list_value = save_data.at("item_list").asValueVector();
    item_list_.clear();
    for (Value item_value : item_list_value) {
        std::string item_data_str = item_value.asString();
        if (item_data_str.size() > 0) {
            auto item = ItemDto::createItemDtoWithString(item_data_str);
            item_list_.push_back(item);
        }
    }
}

void AccountData::resetRoguePlayData() {
    CCLOG("%s %d resetRoguePlayData ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();
    
    // TODO: ここじゃない感がある。装備全解除
    auto it = item_list_.begin();
    while (it != item_list_.end()) {
        (*it).setEquip(false);
        it++;
    }
    
    save();
}

void AccountData::resetAll() {
    CCLOG("%s %d resetAll ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();
    clearItemList();
    
    remoteSaveFile();
}


#pragma mark
#pragma mark 汎用

std::string AccountData::createQuestSaveDetailText() const {
    return cocos2d::StringUtils::format("%s（%d F）\nLv %d exp %d HP %d/%d 所持金 %d G",
                        "初心者の洞窟",
                        this->rogue_play_data_.quest_id,
                        this->player_actor_.getLv(),
                        this->player_actor_.getExp(),
                        this->player_actor_.getHitPoint(),
                        this->player_actor_.getHitPointLimit(),
                        this->player_actor_.getGold());
}


#pragma mark
#pragma mark チェック系

bool AccountData::isQuestSaveData() const {
    if (this->rogue_play_data_.quest_id > 0) {
        return true;
    }
    return false;
}


///////////////////////////////////////////////

static std::string createSaveFilePath() {
    return FileUtils::getInstance()->getWritablePath() + "save_data.plist";
}

static void remoteSaveFile() {
    std::string save_file_path = createSaveFilePath();
    remove(save_file_path.c_str());
}

static void clearRoguePlayData() {
    AccountData::getInstance()->rogue_play_data_ = RogueScene::createRoguePlayData();
}
static void clearPlayerActorData() {
    AccountData::getInstance()->player_actor_ = ActorDto();
}
static void clearItemList() {
    AccountData::getInstance()->item_list_.clear();
}

NS_ROGUE_END

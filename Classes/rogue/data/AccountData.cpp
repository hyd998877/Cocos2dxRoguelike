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

AccountData::AccountData()
: _roguePlayData({0, 0, RogueScene::GameStatus::INIT, 0, 0, 0})
, _playerActor(ActorDto())
, _itemInventory("所持品", 0, RogueGameConfig::USE_ITEM_MAX)
, _itemInventoryStock("倉　庫", 0, RogueGameConfig::STOCK_ITEM_MAX)
{
};

AccountData::~AccountData()
{
};

void AccountData::init()
{
    load();
}

#pragma mark
#pragma mark static method

void AccountData::save(const RogueScene::RoguePlayData& roguePlayData,
                       const ActorDto& playerActor,
                       const ItemInventoryDto& itemInventory)
{
    this->_roguePlayData = roguePlayData;
    this->_playerActor = playerActor;
    this->_itemInventory = itemInventory;
    save();
}

void AccountData::save()
{
    CCLOG("%s %d save ", __FILE__, __LINE__);

    ValueVector saveItemList = createSaveItemList(this->_itemInventory);
    ValueVector saveItemStockList = createSaveItemList(this->_itemInventoryStock);
    
    ValueMap save_data{
        {"rogue_play_data", Value(RogueScene::roguePlayDataToString(_roguePlayData))},
        {"player_actor", Value(_playerActor.actorToString())},
        {"player_actor.weapon_equip", Value(_playerActor.getWeaponEquip().actorEquipToString())},
        {"player_actor.accessory_equip", Value(_playerActor.getAccessoryEquip().actorEquipToString())},
        {"item_list", Value(saveItemList)},
        {"item_stock_list", Value(saveItemStockList)}
    };
    
    // 書き込み
    FileUtils::getInstance()->writeToFile(save_data, createSaveFilePath());
}

ValueVector AccountData::createSaveItemList(const ItemInventoryDto& itemInventory)
{
    ValueVector saveItemList;
    
    auto itemList = itemInventory.getItemList();
    auto it = itemList.begin();
    for (int i = 0; i < itemList.size(); i++) {
        if (it != itemList.end()) {
            auto item_str = (*(it)).itemDtoToString();
            it++;
            saveItemList.push_back(Value(item_str));
        } else {
            // 空データ保存
            saveItemList.push_back(Value(""));
        }
    }
    return saveItemList;
}

void AccountData::load()
{
    std::string save_file_path = createSaveFilePath();
    CCLOG("%s %d load %s ", __FILE__, __LINE__, save_file_path.c_str());
    
    ValueMap save_data = FileUtils::getInstance()->getValueMapFromFile(save_file_path);
    if (save_data.empty()) {
        return;
    }
    
    Value _roguePlayDatavalue = save_data.at("rogue_play_data");
    std::string _roguePlayDatastr = _roguePlayDatavalue.asString();
    if (_roguePlayDatastr.size() > 0) {
        _roguePlayData = RogueScene::createRoguePlayData(_roguePlayDatastr);
    }

    Value _playerActordata_value = save_data.at("player_actor");
    std::string _playerActordata_str = _playerActordata_value.asString();
    if (_playerActordata_str.size() > 0) {
        _playerActor = ActorDto::createActorDto(_playerActordata_str);
    }
    
    Value _playerActorweapon_equip_data_value = save_data.at("player_actor.weapon_equip");
    std::string _playerActorweapon_equip_data_str = _playerActorweapon_equip_data_value.asString();
    if (_playerActorweapon_equip_data_str.size() > 0) {
        _playerActor.setWeaponEquip(ActorEquipDto::createEquipDto(_playerActorweapon_equip_data_str));
    }
    
    Value _playerActoraccessory_equip_data_value = save_data.at("player_actor.accessory_equip");
    std::string _playerActoraccessory_equip_data_str = _playerActoraccessory_equip_data_value.asString();
    if (_playerActoraccessory_equip_data_str.size() > 0) {
        _playerActor.setAccessoryEquip(ActorEquipDto::createEquipDto(_playerActoraccessory_equip_data_str));
    }
    
    ValueVector item_list_value = save_data.at("item_list").asValueVector();
    std::list<ItemDto> itemList;
    for (Value item_value : item_list_value) {
        std::string item_data_str = item_value.asString();
        if (item_data_str.size() > 0) {
            auto item = ItemDto::createItemDtoWithString(item_data_str);
            itemList.push_back(item);
        }
    }
    this->_itemInventory.setItemList(itemList);
}

void AccountData::resetRoguePlayData() {
    CCLOG("%s %d resetRoguePlayData ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();
    
    this->_itemInventory.resetAllEquip();
    
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
    return cocos2d::StringUtils::format("%s（%d F）\nLv %d exp %d HP %d/%d 所持金 %ld G",
                        "初心者の洞窟",
                        this->_roguePlayData.quest_id,
                        this->_playerActor.getLv(),
                        this->_playerActor.getExp(),
                        this->_playerActor.getHitPoint(),
                        this->_playerActor.getHitPointLimit(),
                        this->_itemInventory.getGold());
}


#pragma mark
#pragma mark チェック系

bool AccountData::isQuestSaveData() const {
    if (this->_roguePlayData.quest_id > 0) {
        return true;
    }
    return false;
}

void AccountData::clearRoguePlayData() {
    this->_roguePlayData = RogueScene::createRoguePlayData();
}
void AccountData::clearPlayerActorData() {
    this->_playerActor = ActorDto();
}
void AccountData::clearItemList() {
    this->_itemInventory.clearItemList();
}

///////////////////////////////////////////////

static std::string createSaveFilePath() {
    return FileUtils::getInstance()->getWritablePath() + "save_data.plist";
}

static void remoteSaveFile() {
    std::string save_file_path = createSaveFilePath();
    remove(save_file_path.c_str());
}

NS_ROGUE_END

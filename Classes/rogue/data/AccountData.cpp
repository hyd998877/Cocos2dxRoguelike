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
, _systemData()
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

void AccountData::saveInventory(const ItemInventoryDto &itemInventory)
{
    this->_itemInventory = itemInventory;
    save();
}

void AccountData::saveInventory(const ItemInventoryDto &itemInventory, const ItemInventoryDto &itemInventoryStock)
{
    this->_itemInventory = itemInventory;
    this->_itemInventoryStock = itemInventoryStock;
    save();
}

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
    int gold = this->_itemInventory.getGold();
    int stockGold = this->_itemInventoryStock.getGold();
    
    ValueMap playScoreMap;
    for (auto mapData : this->_systemData.getPlayScoreMap()) {
        playScoreMap.insert({mapData.first, Value(mapData.second)});
    }
    
    ValueMap save_data{
        {"rogue_play_data", Value(RogueScene::roguePlayDataToString(_roguePlayData))},
        {"player_actor", Value(this->_playerActor.actorToString())},
        {"player_actor.weapon_equip", Value(this->_playerActor.getWeaponEquip().actorEquipToString())},
        {"player_actor.accessory_equip", Value(this->_playerActor.getAccessoryEquip().actorEquipToString())},
        {"inventory.gold", Value(gold)},
        {"inventory.item_list", Value(saveItemList)},
        {"inventoryStock.gold", Value(stockGold)},
        {"inventoryStock.item_list", Value(saveItemStockList)},
        {"system", Value(this->_systemData.toSeparatedString())},
        {"system.play_score_map", Value(playScoreMap)}
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
    
    Value roguePlayDatavalue = save_data.at("rogue_play_data");
    std::string roguePlayDatastr = roguePlayDatavalue.asString();
    if (!roguePlayDatastr.empty()) {
        this->_roguePlayData = RogueScene::createRoguePlayData(roguePlayDatastr);
    }

    Value playerActordata_value = save_data.at("player_actor");
    std::string playerActordata_str = playerActordata_value.asString();
    if (!playerActordata_str.empty()) {
        this->_playerActor = ActorDto::createActorDto(playerActordata_str);
    }
    
    Value playerActorweapon_equip_data_value = save_data.at("player_actor.weapon_equip");
    std::string playerActorweapon_equip_data_str = playerActorweapon_equip_data_value.asString();
    if (!playerActorweapon_equip_data_str.empty()) {
        this->_playerActor.setWeaponEquip(ActorEquipDto::createEquipDto(playerActorweapon_equip_data_str));
    }
    
    Value playerActoraccessory_equip_data_value = save_data.at("player_actor.accessory_equip");
    std::string playerActoraccessory_equip_data_str = playerActoraccessory_equip_data_value.asString();
    if (!playerActoraccessory_equip_data_str.empty()) {
        this->_playerActor.setAccessoryEquip(ActorEquipDto::createEquipDto(playerActoraccessory_equip_data_str));
    }
  
    {
        int gold = save_data.at("inventory.gold").asInt();
        this->_itemInventory.setGold(gold);
        
        ValueVector item_list_value = save_data.at("inventory.item_list").asValueVector();
        std::list<ItemDto> itemList;
        for (Value item_value : item_list_value) {
            std::string item_data_str = item_value.asString();
            if (!item_data_str.empty()) {
                auto item = ItemDto::createItemDtoWithString(item_data_str);
                itemList.push_back(item);
            }
        }
        this->_itemInventory.setItemList(itemList);
    }
    
    {
        int stockGold = save_data.at("inventoryStock.gold").asInt();
        this->_itemInventoryStock.setGold(stockGold);
        
        ValueVector itemStockListValue = save_data.at("inventoryStock.item_list").asValueVector();
        std::list<ItemDto> itemStockList;
        for (Value itemValue : itemStockListValue) {
            std::string itemDataStr = itemValue.asString();
            if (!itemDataStr.empty()) {
                auto item = ItemDto::createItemDtoWithString(itemDataStr);
                itemStockList.push_back(item);
            }
        }
        this->_itemInventoryStock.setItemList(itemStockList);
    }
    
    Value system_data_value = save_data.at("system");
    std::string system_data_value_str = system_data_value.asString();
    if (!system_data_value_str.empty()) {
        this->_systemData = SystemDataDto::createWithSeparatedString(system_data_value_str);
    }
    
    Value system_data_play_score_map_value = save_data.at("system.play_score_map");
    auto system_data_play_score_map_value_map = system_data_play_score_map_value.asValueMap();
    SystemDataDto::PlayScoreMap playScoreMap;
    for (auto data : system_data_play_score_map_value_map) {
        playScoreMap.insert({data.first, data.second.asInt()});
    }
    this->_systemData.setPlayScoreMap(playScoreMap);
}

// もちかえり
void AccountData::resetRoguePlayData() {
    CCLOG("%s %d resetRoguePlayData ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();

    // 装備は解除
    this->_itemInventory.resetAllEquip();
    
    save();
}

// 死亡
void AccountData::resetRoguePlayDataAndInventory() {
    CCLOG("%s %d resetRoguePlayDataAndInventory ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();
    clearInventory();
    
    save();
}

void AccountData::resetAll() {
    CCLOG("%s %d resetAll ", __FILE__, __LINE__);
    
    clearRoguePlayData();
    clearPlayerActorData();
    clearInventory();
    clearInventoryStock();
    clearSystemData();
    
    remoteSaveFile();
}

#pragma mark
#pragma mark 汎用

std::string AccountData::createQuestSaveDetailText() const {
    // TODO: ダンジョン名が仮
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

const ItemInventoryDto &AccountData::changeInventoryItem(long objectId)
{
    if (this->_itemInventory.isInventoryByObjectId(objectId)) {
        this->_itemInventoryStock.addItemDto(this->_itemInventory.findByObjectId(objectId));
        this->_itemInventory.removeItemDto(objectId);
        return this->getItemInventory();
    } else if (this->_itemInventoryStock.isInventoryByObjectId(objectId)) {
        this->_itemInventory.addItemDto(this->_itemInventoryStock.findByObjectId(objectId));
        this->_itemInventoryStock.removeItemDto(objectId);
        return this->getItemInventoryStock();
    }
    return this->getItemInventory();
}

#pragma mark
#pragma mark System系

void AccountData::gameObjectCountUp()
{
    this->_systemData.setGameObjectCount(this->_systemData.getGameObjectCount() + 1);
}

long AccountData::getGameObjectId() const
{
    return this->_systemData.getGameObjectCount() + 1;
}

void AccountData::putPlayScore(const std::string &questKey, int score)
{
    auto value = this->_systemData.getPlayScoreMap()[questKey];
    if (score > value) {
        this->_systemData.putPlayScoreMap(questKey, score);
    }
}

AccountData::GamePlayProgress AccountData::getGamePlayProgress()
{
    return static_cast<AccountData::GamePlayProgress>(this->_systemData.getGameProgress());
}

void AccountData::updateGamePlayProgress(AccountData::GamePlayProgress progress)
{
    if (this->_systemData.getGameProgress() < progress) {
        this->_systemData.setGameProgress(progress);
    }
    save();
}

///////////////////////////////////////////////
// private
void AccountData::clearRoguePlayData()
{
    this->_roguePlayData = RogueScene::createRoguePlayData();
}
void AccountData::clearPlayerActorData()
{
    this->_playerActor = ActorDto();
}
void AccountData::clearInventory()
{
    this->_itemInventory = ItemInventoryDto("所持品", 0, RogueGameConfig::USE_ITEM_MAX);
}
void AccountData::clearInventoryStock()
{
    this->_itemInventoryStock = ItemInventoryDto("倉　庫", 0, RogueGameConfig::STOCK_ITEM_MAX);
}
void AccountData::clearSystemData()
{
    this->_systemData = SystemDataDto();
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

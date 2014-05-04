//
//  AccountData.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#ifndef __Cocos2dxSRPGQuest__AccountData__
#define __Cocos2dxSRPGQuest__AccountData__

#include "cocos2d.h"

#include "RogueScene.h"
#include "MapManager.h"

#include "ActorDto.h"
#include "Itemdto.h"
#include "ItemInventoryDto.h"

NS_ROGUE_BEGIN

class AccountData
{
public:
    // シングルトン
    static AccountData* getInstance();

    // --- システムデータ ---
    // TODO: (kyokomi)別クラスかなぁ
    
    const RogueLikeGame::RogueScene::RoguePlayData &getRoguePlayData() { return _roguePlayData; }
    const ActorDto &getPlayerActor() { return _playerActor; }
    const ItemInventoryDto &getItemInventory() { return _itemInventory; }
    const ItemInventoryDto &getItemInventoryStock() { return _itemInventoryStock; }
    
    void load();
    void save(const RogueLikeGame::RogueScene::RoguePlayData &roguePlayData,
              const ActorDto &playerActor,
              const ItemInventoryDto &itemInventory);
    void save();
    void saveInventory(const ItemInventoryDto &itemInventory);
    void saveInventory(const ItemInventoryDto &itemInventory, const ItemInventoryDto &itemInventoryStock);

    void resetRoguePlayData();
    void resetAll();
    
    std::string createQuestSaveDetailText() const;
    // チェック系
    bool isQuestSaveData() const;
    
    // イベントリ系
    const ItemInventoryDto &changeInventoryItem(long objectId);
private:
    
    AccountData();
    ~AccountData();
    
    void init();
    
    static cocos2d::ValueVector createSaveItemList(const ItemInventoryDto& itemInventory);
    
    // --- ローグプレイ情報 ---
    RogueLikeGame::RogueScene::RoguePlayData _roguePlayData;
    // --- プレイヤー情報 ---
    ActorDto _playerActor;
    // --- インベントリ ---
    // 所持品、装備状態
    ItemInventoryDto _itemInventory;
    // --- 倉庫 ---
    ItemInventoryDto _itemInventoryStock;
    
    void clearRoguePlayData();
    void clearPlayerActorData();
    void clearItemList();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__AccountData__) */

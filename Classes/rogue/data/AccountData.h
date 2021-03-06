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

#include "ActorDto.h"
#include "ItemInventoryDto.h"
#include "SystemDataDto.h"

NS_ROGUE_BEGIN

class AccountData
{
public:
    enum GamePlayProgress {
        INIT             =  0,
        TUTORIAL_PLAY    = 10,
        TUTORIAL_CLEAR   = 11,
        MAINQUEST_PLAY   = 20,
        MAINQUEST_CLEAR  = 21,
        MAINQUEST2_CLEAR = 22,
        DEEPQUEST_CLEAR  = 23,
    };
    std::string playedBGMKey;
    
    // シングルトン
    static AccountData* getInstance();
    
    const RoguePlayDto &getRoguePlayData() { return _roguePlayData; }
    const ActorDto &getPlayerActor() { return _playerActor; }
    const ItemInventoryDto &getItemInventory() { return _itemInventory; }
    const ItemInventoryDto &getItemInventoryStock() { return _itemInventoryStock; }
    
    void load();
    void save(const RoguePlayDto &roguePlayData,
              const ActorDto &playerActor,
              const ItemInventoryDto &itemInventory);
    void save();
    void saveInventory(const ItemInventoryDto &itemInventory);
    void saveInventory(const ItemInventoryDto &itemInventory, const ItemInventoryDto &itemInventoryStock);

    void resetRoguePlayDataAndInventory();
    void resetRoguePlayData();
    void resetAll();
    
    std::string createQuestSaveDetailText() const;
    // チェック系
    bool isItemInventory();
    bool isQuestSaveData() const;
    bool isPlayQuestData(RoguePlayDto::QuestType questType, int questId) const;
    
    // system系
    void gameObjectCountUp();
    long getGameObjectId() const;
    void putPlayScore(const std::string &questKey, int score);
    AccountData::GamePlayProgress getGamePlayProgress();
    void clearQuestTypeWithUpdateGamePlayProgress(RoguePlayDto::QuestType questType);
    void updateGamePlayProgress(AccountData::GamePlayProgress progress);
    
    // イベントリ系
    const ItemInventoryDto &changeInventoryItem(long objectId);
    
private:
    
    AccountData();
    ~AccountData();
    
    void init();
    
    static cocos2d::ValueVector createSaveItemList(const ItemInventoryDto& itemInventory);
    
    // --- ローグプレイ情報 ---
    RoguePlayDto _roguePlayData;
    // --- プレイヤー情報 ---
    ActorDto _playerActor;
    // --- インベントリ ---
    // 所持品、装備状態
    ItemInventoryDto _itemInventory;
    // --- 倉庫 ---
    ItemInventoryDto _itemInventoryStock;
    // --- システムデータ ---
    SystemDataDto _systemData;
    
    void clearRoguePlayData();
    void clearPlayerActorData();
    void clearInventory();
    void clearInventoryStock();
    void clearSystemData();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__AccountData__) */

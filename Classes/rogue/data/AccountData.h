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

    // --- ローグプレイ情報 ---
    RogueLikeGame::RogueScene::RoguePlayData rogue_play_data_;
    
    // --- プレイヤー情報 ---
    ActorDto player_actor_;
    
    // --- インベントリ ---
    // 所持品、装備状態
    ItemInventoryDto _itemInventory;
    
    // --- 倉庫 ---
    // アイテムリスト
    // ゴールド
    // TODO: (kyokomi)別クラスかなぁ
    
    // --- システムデータ ---
    // TODO: (kyokomi)別クラスかなぁ

    void load();
    void save();
    void resetRoguePlayData();
    void resetAll();
    
    std::string createQuestSaveDetailText() const;
    // チェック系
    bool isQuestSaveData() const;
private:
    
    AccountData();
    ~AccountData();
    
    void init();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__AccountData__) */

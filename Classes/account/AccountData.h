//
//  AccountData.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#ifndef __Cocos2dxSRPGQuest__AccountData__
#define __Cocos2dxSRPGQuest__AccountData__

#include <iostream>
#include <list>

#include "RogueScene.h"

#include "ActorSprite.h"
#include "DropItemSprite.h"

class AccountData
{
public:
    // シングルトン
    static AccountData* getInstance();
    
    // --- プレイデータ ---
    
    // マップの配置情報
    // TODO: MapManagerで持ってるからなぁ。。。

    // --- ローグプレイ情報 ---
    RogueScene::RoguePlayData rogue_play_data_;
    
    // --- プレイヤー情報 ---
    ActorSprite::ActorDto player_actor_;
    
    // --- インベントリ ---
    // 所持品、装備状態
    std::list<DropItemSprite::DropItemDto> item_list_;
    
    // --- 倉庫 ---
    // アイテムリスト
    // ゴールド
    // TODO: (kyokomi)別クラスかなぁ
    
    // --- システムデータ ---
    // TODO: (kyokomi)別クラスかなぁ
    
public:
    void load();
    void save();
    
private:
    AccountData()
    :rogue_play_data_(),
    player_actor_(),
    item_list_(std::list<DropItemSprite::DropItemDto>())
    {
    };
    
    ~AccountData()
    {
        item_list_.clear();
    };
    
    void init();
};

#endif /* defined(__Cocos2dxSRPGQuest__AccountData__) */

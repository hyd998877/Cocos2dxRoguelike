//
//  RogueScene.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/02.
//
//

#ifndef __Cocos2dxSRPGQuest__RogueScene__
#define __Cocos2dxSRPGQuest__RogueScene__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

#include "MapManager.h"
#include "ActorSprite.h"

#include "RogueTMXTiledMap.h"

#include "ItemInventoryDto.h"
#include "RoguePlayDto.h"

USING_NS_CC;

class KeypadLayout;
class HeaderStatusLayout;
class GameLogLayout;

NS_ROGUE_BEGIN

class RogueScene : public cocos2d::Layer
{
public:
    RogueScene();
    virtual ~RogueScene();
    
    // 初期化とか
    virtual bool initWithQuestId(RoguePlayDto::QuestType questType, int questId);
    
    static cocos2d::Scene* scene(RoguePlayDto::QuestType questType, int questId);
    static RogueScene* createWithQuestId(RoguePlayDto::QuestType questType, int questId);
        
    void onEnter();
    void onEnterTransitionDidFinish();
    
private:
    void changeGameStatus(RoguePlayDto::GameStatus gameStatus);
    void enemyTurn();
    void checkEnmeyTurnEnd();
    
    float getAnimationSpeed();
    void changeScene(cocos2d::Scene* scene);
    void refreshStatus();
    void refreshStatusEquip(const ActorDto& actorDto);
    
    // タッチイベント系
    bool isKeypadControll();
    void touchEventExec(cocos2d::Vec2 touchPoint);
    void touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex);
    void touchKaidan();
    void touchDropItem(const DropMapItem& drop_map_item);
    
    void attack();
    void attackCallback(ActorSprite* player, ActorSprite* enemy);
    
    // UI関連
    
    // カットイン再生
    void playFloorTitleCutIn(int questId);
    void playGameOverCutIn();
    
    // イベントリ表示
    void showItemInventoryWindow();
    void itemWindowDropItem(const ItemDto &itemDto);
    void itemWindowEquipItem(const ItemDto &itemDto);
    void itemWindowUseItem(const ItemDto &itemDto);
    
    // システムメニュ
    void showSystemMenu();
    void hideSystemMenu();
    
    // 配置
    void institutionEnemy(int probCount);
    void institutionDropItem(int probCount, const MapIndex& mapIndex = MapManager::createMapIndexEmpty());
    
    // 汎用
    const ValueMap getRogueMapData();
    ActorSprite* getPlayerActorSprite(int seqNo);
    RogueTMXTiledMap* getRogueMapLayer();
    
    // 照明
    void rogueMapLighting();
    void showPlayerLighting(ActorSprite* actor_sprite);
    void hidePlayerLighting();
    
    // 呪文
    void enemyMappingAllShow();
    void itemMappingAllShow();
    void floorMappingAllShow();
    
    void setupKeypadLayout();
    MapIndex getNowRogueMapIndex();
    
    ///////////
    MapManager* getMapManager();
private:
    enum Tags {
        TiledMapLayerTag          =      1,
        GridLineTag               =    100,
        ActorPlayerTag            = 100000,
        RoguePlayerLightBaseTag   = 110000,  // プレイヤー周辺の明かりを持つベースLayer
        RoguePlayerLightTag       = 110001,  // 明る部分
        RoguePlayerLightMaskTag   = 110002,  // 暗い部分
        MiniMapLayerTag           = 150000,
        GameLogLayerTag           = 210000,
        ItemListWindowTag         = 220000,
        CommonWindowTag           = 230000,
        SystemMenuWindowTag       = 240000,
        ModalLayerTag             = 900000,
    };
    
    enum ZOrders {
        TiledMapLayerZOrder = 1,
        GridLineZOrder,
        ActorBaseZOrder,
        ActorPlayerZOrder,
        ActionCursorZOrder,
        RoguePlayerLightBaseZOrder,    // プレイヤー周辺の明かりを持つベースLayer
        RoguePlayerLightZOrder,        // 明るい部分
        RoguePlayerLightMaskZOrder,    // 暗い部分
        MiniMapLayerZOrder,
        StatusBarLayerZOrder,
        GameLogLayerZOrder,
        MenuLayerZOrder,
        ItemListLayerZOrder,
        CommonWindowZOrder,
        SystemMEnuWindowZOrder,
        CutInLayerZOrder,
        ModalLayerZOrder,
        BlackLayerZOrder
    };
    
    // ゲーム管理 save
    RoguePlayDto::GameStatus _beforeGameStatus;
    RoguePlayDto::GameStatus _gameStatus;
    RoguePlayDto _roguePlayDto;
    // 所持アイテム
    ItemInventoryDto _itemInventory;
    
    MapManager _mapManager;
    
    // --------- cocostudio ---------
    
    KeypadLayout* _keypadLayout;
    HeaderStatusLayout* _statusWidget;
    GameLogLayout* _gameLogWidget;
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__RogueScene__) */

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
#include "DropItemSprite.h"
#include "ItemWindowLayer.h"
#include "AlertDialogLayer.h"
#include "ModalLayer.h"

class RogueScene : public cocos2d::Layer
{
    static const int MAX_LOG_LENGTH = 16*1024;
protected:
    // ゲームステート
    enum GameStatus {
        INIT           = 0,
        PLAYER_TURN    = 10,
        PLAYER_ACTION  = 11,
        PLAYER_NO_ACTION  = 12,
        ENEMY_TURN     = 20,
        ENEMY_ACTION   = 21,
        GAME_OVER      = 99,
    };
    
    // 廃止というかkやめる
    enum TiledMapTag {
        kGridLineTag             = 1000,
        kTiledMapDropItemBaseTag = 10000, // + seqNo
        kTiledMapObjectTag       = 15000, // 階段専用
        kTiledMapEnemyBaseTag    = 20000, // + seqNo
        
        TiledMapFrontLayerTag          = 40000,
        FloorLayerTag                  = 40001,
        FloorMaskLayerTag              = 40002,
        FloorMaskPlayerLayerTag        = 40003,
    };
    
    // 廃止というかzやめる
    enum TiledMapIndex {
        zGridLineIndex = 1,
        zTiledMapDropItemBaseIndex,
        zTiledMapObjectIndex,
        zTiledMapEnemyBaseIndex,
        TiledMapFrontZOrder,
        FloorLayerZOrder,
        FloorMaskLayerZOrder,
//        FloorMaskPlayerLayerZOrder
    };
    
    // 廃止予定
    enum _Tag {
        kTiledMapTag          = 1,
//        kCursorBaseTag      = 9000,
//        kCursorMoveFindTag  = 9001,
//        kCursorMoveStepTag  = 9002,
//        kCursorSelectedTag  = 9003,
//        kActorBaseTag         = 100000,
//        kMiniMapTag           = 150000,
        kStatusBarTag         = 200000,
//        kStatusBar2Tag        = 200001,
        kGameLogTag           = 210000,
//        kItemListTag          = 220000,
//        kMenuTag              = 300000,
//        kModalTag             = 900000,
    };
    
    // ミニマップ上のタグ
    enum MiniMapLayerTag {
        MiniMapSimbolBatchNodeTag = 100,
    };
    
    enum Tag {
        ActorPlayerTag            = 100000,
        RogueLayerTag             = 110000,
//        RogueMaskLayerTag         = 110001,
        MiniMapLayerTag           = 150000,
        ItemListWindowTag         = 220000,
        CommonWindowTag           = 230000,
        KeypadMenuTag             = 300000,
        ButtonMenuTag             = 310000,
        A_ButtonMenuTag           = 310001,
        B_ButtonMenuTag           = 310002,
        C_ButtonMenuTag           = 310003,
        D_ButtonMenuTag           = 310004,
        ModalLayerTag             = 900000,
    };
    
    enum ZOrder {
        TiledMapLayerZOrder = 1,
        RogueZOrder,
        RogueMaskZOrder,
        ActorBaseZOrder,
        ActorPlayerZOrder,
        ActionCursorZOrder,
        MiniMapLayerZOrder,
        MiniMapLayerMapNoneZOrder,
        MiniMapLayerMapItemZOrder,
        MiniMapLayerMapActorZOrder,
        StatusBarLayerZOrder,
        GameLogLayerZOrder,
        MenuLayerZOrder,
        ItemListLayerZOrder,
        CommonWindowZOrder,
        ModalLayerZOrder,
    };
    
    cocos2d::EventListener* m_listener;
    
private:
    int m_questId;
    // ゲーム管理
    GameStatus m_gameStatus;
    int m_noActionCount;
    int m_TurnCount;
    int m_enemyCount;
    bool m_isSppedUp;
    
    float getAnimationSpeed();
    void changeGameStatus(GameStatus gameStatus);
    void enemyTurn();
    void checkEnmeyTurnEnd();
    
    void changeScene(cocos2d::Scene* scene);
    
    // マップベース情報
    cocos2d::Size m_baseContentSize;
    cocos2d::Size m_baseMapSize;
    cocos2d::Size m_baseTileSize;

    // マップ制御
    MapManager m_mapManager;
    MapIndex getRandomMapIndex(bool isColision, bool isActor);
    
    // タッチイベント系
    void touchEventExec(cocos2d::Point touchPoint);
    void touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex);
    void attack();
    
    MapIndex checkTouchEventIndex(MapIndex touchPointMapIndex);
    void moveMap(MapIndex addMoveIndex, int actorSeqNo, MapDataType mapDataType, cocos2d::CallFunc* moveFinishedCallFunc);
    bool isTiledMapColisionLayer(MapIndex touchPointMapIndex);
    bool isMapLayerOver(MapIndex touchPointMapIndex);
    
    // UI関連
    void logMessage(const char * pszFormat, ...);
    DrawNode* createGridDraw();
    Vector<MenuItem*> createKeypadMenuItemArray();
    Vector<MenuItem*> createButtonMenuItemArray();
    
    MenuItem* createKeypadMenuItemSprite(SpriteFrame* pBaseSpriteFrame, SpriteFrame* pBasePressSpriteFrame, const ccMenuCallback& callback);
    
    // アイテムリスト
    void showItemList(int showTextIndex);
    void hideItemList();
    void refreshStatus();
    
    void showCommonWindow(std::string titleText, const ccMenuCallback& okMenuItemCallback, const ccMenuCallback& ngMenuItemCallback);
//    void showCommonWindow(std::string title, std::string okText, std::string ngText);
    AlertDialogLayer* getCommonWindow();
    void hideCommonWindow();
    
    // tileSet関連
    bool tileSetEnemyActorMapItem(ActorSprite::ActorDto enemyActorDto, MapIndex mapIndex);
    bool tileSetDropMapItem(DropItemSprite::DropItemDto dropItemDto, MapIndex mapIndex);
    
    void removeEnemyActorSprite(ActorSprite* pActorSprite);
    void removeDropItemSprite(Node* pRemoveParentNode, DropItemSprite* pDropItemSprite);
    
    // マップ座標変換
    cocos2d::Point indexToPoint(int mapIndex_x, int mapIndex_y);
    cocos2d::Point indexToPoint(MapIndex mapIndex);
    cocos2d::Point indexToPointNotTileSize(int mapIndex_x, int mapIndex_y);
    cocos2d::Point indexToPointNotTileSize(MapIndex mapIndex);
    
    MapIndex pointToIndex(cocos2d::Point point);
    MapIndex touchPointToIndex(cocos2d::Point point);
    // MapIndexからtileIndexに変換
    MapIndex mapIndexToTileIndex(int mapIndex_x, int mapIndex_y);
    MapIndex mapIndexToTileIndex(MapIndex mapIndex);
    
    ActorSprite* getPlayerActorSprite(int seqNo);
    ActorSprite* getEnemyActorSprite(int seqNo);
    ModalLayer* getModalLayer();
    ItemWindowLayer* getItemWindowLayer();
    
    // ミニマップ関連
    void addMiniMapItem(MapItem* pMapItem, int baseSpriteTag);
    
    // 照明
    void tiledMapLighting();
    Rect getTileMapFloorInfo();
    void showPlayerLighting();
    void hidePlayerLighting();
    void showFloorLighting(const Rect floorInfoIndexRect);
    void hideFloorLighting();
    void tiledMapItemLighting(const Rect& floorInfoIndexRect, bool isRefresh);
    Rect createPlayerRect(int rectSize);
    void refreshAutoMapping(const Rect& floorInfoIndexRect);
    
    SpriteBatchNode* getGridSpriteBatchNode();
    
    
public:
    RogueScene();
    virtual ~RogueScene();
    
    // 初期化とか
    virtual bool initWithQuestId(int questId);
    
    static cocos2d::Scene* scene(int questId);
    static RogueScene* createWithQuestId(int questId);
        
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__Cocos2dxSRPGQuest__RogueScene__) */

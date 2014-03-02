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
public:
    // ゲームステート
    enum GameStatus {
        INIT              = 0,
        PLAYER_TURN       = 10,
        PLAYER_ACTION     = 11,
        PLAYER_NO_ACTION  = 12,
        ENEMY_TURN        = 20,
        ENEMY_ACTION      = 21,
        GAME_OVER         = 99,
    };
    
    typedef struct _RoguePlayData {
        // クエストID
        int quest_id;
        // フロアID
        int floor_id;
        // ゲーム状態
        RogueScene::GameStatus game_status;
        // 未行動カウント(足踏み自動回復とかの）
        int no_action_count;
        // ターン数
        int turn_count;
        // 敵出現数（トータル）
        int enemy_count;
    } RoguePlayData;
    
protected:
    
    static const int MAX_LOG_LENGTH = 16*1024;
    
    enum TiledMapTag {
        TiledMapDropItemBaseTag = 10000, // + seqNo
        TiledMapObjectTag       = 15000, // 階段専用
        TiledMapEnemyBaseTag    = 20000, // + seqNo
        
        TiledMapFrontLayerTag          = 40000,
        FloorLayerTag                  = 40001,
        FloorMaskLayerTag              = 40002,
        FloorMaskPlayerLayerTag        = 40003,
    };
    
    enum TiledMapIndex {
        TiledMapDropItemBaseZOrder,
        TiledMapObjectZOrder,
        TiledMapEnemyBaseZOrder,
        TiledMapFrontZOrder,
        FloorLayerZOrder,
        FloorMaskLayerZOrder,
    };
    
    // ミニマップ上のタグ
    enum MiniMapLayerTag {
        MiniMapSimbolBatchNodeTag = 100,
    };
    
    enum Tag {
        TiledMapLayerTag          =      1,
        GridLineTag               =    100,
        ActorPlayerTag            = 100000,
        RogueLayerTag             = 110000,
        MiniMapLayerTag           = 150000,
        StatusBarLayerTag         = 200000,
        GameLogLayerTag           = 210000,
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
        GridLineZOrder,
        RogueZOrder,               // 明る部分
        RogueMaskZOrder,           // 暗い部分
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
    
private:
    // ゲーム管理 save
    RoguePlayData rogue_play_data_;
    
    // マップベース情報 （フロアIDから取れるのでsave不要）
    cocos2d::Size base_content_size_;
    cocos2d::Size base_map_size_;
    cocos2d::Size base_tile_size_;
    
private:
    float getAnimationSpeed();
    void changeGameStatus(GameStatus gameStatus);
    void enemyTurn();
    void checkEnmeyTurnEnd();
    
    void changeScene(cocos2d::Scene* scene);
    
    // マップ制御
//    MapManager m_mapManager; // TODO: シングルトンにしないとな あと再構築に必要なデータを取得するメソッド
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

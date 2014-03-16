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
#include "RogueTMXTiledMap.h"

USING_NS_CC;

using namespace rogue;

#define MINI_MAP_SCALE (4.16f / GAME_SCALE)

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
        // クエストID（階数）
        int quest_id;
        // フロアID(マップデータID)
        int floor_id;
        // ゲーム状態
        RogueScene::GameStatus game_status;
        // 未行動カウント(足踏み自動回復とかの）
        int no_action_count;
        // ターン数
        int turn_count;
        // 敵出現数（トータル）
        int enemy_count;
        // アイテム出現数（トータル）
        int item_count;
        
    } RoguePlayData;
    
    static RoguePlayData createRoguePlayData(std::string data_string)
    {
        std::vector<std::string> data_string_array = StringUtil::split(std::move(data_string));
        if (data_string_array.size() != 7) {
            return {0, 0, GameStatus::INIT, 0, 0, 0};
        }
        
        int index = 0;
        RoguePlayData data;
        data.quest_id = atoi(data_string_array[index].c_str()); index++;
        data.floor_id = atoi(data_string_array[index].c_str()); index++;
        data.game_status = static_cast<GameStatus>(atoi(data_string_array[index].c_str())); index++;
        data.no_action_count = atoi(data_string_array[index].c_str()); index++;
        data.turn_count = atoi(data_string_array[index].c_str()); index++;
        data.enemy_count = atoi(data_string_array[index].c_str()); index++;
        data.item_count = atoi(data_string_array[index].c_str()); index++;
        return data;
    }
    
    static std::string const roguePlayDataToString(const RoguePlayData& data)
    {
        return StringUtils::format("%d,%d,%d,%d,%d,%d,%d",
                                   data.quest_id,
                                   data.floor_id,
                                   data.game_status,
                                   data.no_action_count,
                                   data.turn_count,
                                   data.enemy_count,
                                   data.item_count
                                   );
    }
    
private:
    static const int MAX_LOG_LENGTH = 16*1024;
    
    enum Tags {
        TiledMapLayerTag          =      1,
        GridLineTag               =    100,
        ActorPlayerTag            = 100000,
        RoguePlayerLightBaseTag   = 110000,  // プレイヤー周辺の明かりを持つベースLayer
        RoguePlayerLightTag       = 110001,  // 明る部分
        RoguePlayerLightMaskTag   = 110002,  // 暗い部分
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
        CutInLayerZOrder,
        ModalLayerZOrder,
    };

    // ゲーム管理 save
    RoguePlayData rogue_play_data_;
    
public:
    RogueScene();
    virtual ~RogueScene();
    
    // 初期化とか
    virtual bool initWithQuestId(int questId);
    
    static cocos2d::Scene* scene(int questId);
    static RogueScene* createWithQuestId(int questId);
        
    void onEnter();
    void onEnterTransitionDidFinish();
    
private:
    void changeGameStatus(GameStatus gameStatus);
    void enemyTurn();
    void checkEnmeyTurnEnd();
    
    float getAnimationSpeed();
    void changeScene(cocos2d::Scene* scene);
    void refreshStatus();
    
    // タッチイベント系
    void touchEventExec(cocos2d::Point touchPoint);
    void touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex);
    MapIndex checkTouchEventIndex(MapIndex touchPointMapIndex);
    
    void attack();
    
    // UI関連
    void logMessage(const char * pszFormat, ...);
    Vector<MenuItem*> createKeypadMenuItemArray();
    Vector<MenuItem*> createButtonMenuItemArray();
    MenuItem* createKeypadMenuItemSprite(SpriteFrame* pBaseSpriteFrame, SpriteFrame* pBasePressSpriteFrame, const ccMenuCallback& callback);
    LayerColor* createFloorTitleCutinLayer(int quest_id);
    
    // アイテムリスト
    void showItemList(int showTextIndex);
    void hideItemList();
    
    // 共通ウィンドウ
    void showCommonWindow(std::string titleText, const ccMenuCallback& okMenuItemCallback, const ccMenuCallback& ngMenuItemCallback);
    AlertDialogLayer* getCommonWindow();
    void hideCommonWindow();

    // 汎用
    ActorSprite* getPlayerActorSprite(int seqNo);
    ModalLayer* getModalLayer();
    RogueTMXTiledMap* getRogueMapLayer();
    ItemWindowLayer* getItemWindowLayer();
    
    // 照明
    void rogueMapLighting();
    void showPlayerLighting(ActorSprite* actor_sprite);
    void hidePlayerLighting();
};

#endif /* defined(__Cocos2dxSRPGQuest__RogueScene__) */

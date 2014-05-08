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

#include "StringUtils.h"

USING_NS_CC;

#define MINI_MAP_SCALE (4.16f / GAME_SCALE)

NS_ROGUE_BEGIN
    
class RogueScene : public cocos2d::Layer
{
public:
    
    enum class QuestType {
        TUTORIAL     = 1, /// > 初心者の洞窟
        MAIN_QUEST   = 2, /// > 不思議な洞窟
        MAIN_QUEST2  = 3, /// > 不思議な洞窟（クリア後）
        DEEP_QUEST   = 4  /// > 最果ての洞窟
    };
    
    const static std::string& findQuestKey(RogueScene::QuestType questType);
    
    // ゲームステート
    enum class GameStatus {
        INIT              = 0,
        PLAYER_TURN       = 10,
        PLAYER_ACTION     = 11,
        PLAYER_NO_ACTION  = 12,
        ENEMY_TURN        = 20,
        ENEMY_ACTION      = 21,
        QUEST_CLEAR       = 97,
        ESCAPE            = 98,
        GAME_OVER         = 99,
    };
    
    // TODO: Dtoにしてclassにする
    typedef struct _RoguePlayData {
        // クエスト種類
        RogueScene::QuestType _questType;
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
        
    } RoguePlayData;
    
    static RoguePlayData createRoguePlayData(const std::string &data_string = "")
    {
        std::vector<std::string> data_string_array = GameCore::StringUtils::split(std::string(data_string));
        if (data_string_array.size() != 7) {
            return {RogueScene::QuestType::TUTORIAL, 0, 0, RogueScene::GameStatus::INIT, 0, 0, 0};
        }
        
        int index = 0;
        RoguePlayData data;
        data._questType = static_cast<RogueScene::QuestType>(atoi(data_string_array[index++].c_str()));
        data.quest_id = atoi(data_string_array[index++].c_str());
        data.floor_id = atoi(data_string_array[index++].c_str());
        data.game_status = static_cast<RogueScene::GameStatus>(atoi(data_string_array[index++].c_str()));
        data.no_action_count = atoi(data_string_array[index++].c_str());
        data.turn_count = atoi(data_string_array[index++].c_str());
        data.enemy_count = atoi(data_string_array[index++].c_str());
        return data;
    }
    
    static std::string const roguePlayDataToString(const RoguePlayData& data)
    {
        return cocos2d::StringUtils::format("%d,%d,%d,%d,%d,%d,%d",
                                   data._questType,
                                   data.quest_id,
                                   data.floor_id,
                                   data.game_status,
                                   data.no_action_count,
                                   data.turn_count,
                                   data.enemy_count
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
        // この辺StatusBarがいまいち
        StatusBarEquipWeaponTag            = 200100,
        StatusBarEquipWeaponSpriteTag      = 200101,
        StatusBarEquipWeaponLabelTag       = 200102,
        StatusBarEquipAccessoryTag         = 200200,
        StatusBarEquipAccessorySpriteTag   = 200201,
        StatusBarEquipAccessoryLabelTag    = 200202,
        GameLogLayerTag           = 210000,
        ItemListWindowTag         = 220000,
        CommonWindowTag           = 230000,
        SystemMenuWindowTag       = 240000,
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
        SystemMEnuWindowZOrder,
        CutInLayerZOrder,
        ModalLayerZOrder,
        BlackLayerZOrder
    };

    // ゲーム管理 save
    RoguePlayData rogue_play_data_;
    // 所持アイテム
    ItemInventoryDto _itemInventory;
    
public:
    RogueScene();
    virtual ~RogueScene();
    
    // 初期化とか
    virtual bool initWithQuestId(QuestType questType, int questId);
    
    static cocos2d::Scene* scene(QuestType questType, int questId);
    static RogueScene* createWithQuestId(QuestType questType, int questId);
        
    void onEnter();
    void onEnterTransitionDidFinish();
    
private:
    void changeGameStatus(GameStatus gameStatus);
    void enemyTurn();
    void checkEnmeyTurnEnd();
    
    float getAnimationSpeed();
    void changeScene(cocos2d::Scene* scene);
    void refreshStatus();
    void refreshStatusEquip(const ActorDto& actorDto);
    
    // タッチイベント系
    void touchEventExec(cocos2d::Point touchPoint);
    void touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex);
    void touchKaidan();
    void touchDropItem(const DropMapItem& drop_map_item);
    
    void attack();
    void attackCallback(ActorSprite* player, ActorSprite* enemy);
    
    // UI関連
    void logMessage(const char * pszFormat, ...);
    Vector<MenuItem*> createKeypadMenuItemArray();
    Vector<MenuItem*> createButtonMenuItemArray();
    MenuItem* createKeypadMenuItemSprite(SpriteFrame* pBaseSpriteFrame, SpriteFrame* pBasePressSpriteFrame, const ccMenuCallback& callback);
    
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
};

NS_ROGUE_END

#endif /* defined(__Cocos2dxSRPGQuest__RogueScene__) */

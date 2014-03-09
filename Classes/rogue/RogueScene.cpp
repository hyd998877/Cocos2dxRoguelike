//
//  RogueScene.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/02.
//
//
#include "AppMacros.h"

#include "RogueScene.h"
// ------------------------------
#include "CommonWindowUtil.h"

#include "TableViewTestLayer.h"

#include "ItemLogic.h"
#include "BattleLogic.h"

#include "TitleSceneLoader.h"

#include "MLevelDao.h"
#include "MWeaponDao.h"
#include "MUseItemDao.h"
#include "MAccessoryDao.h"

#include "AccountData.h"

USING_NS_CC;

// プロトタイプ宣言
int GetRandom(int min,int max);

std::size_t f_r(const std::string& s, char c)
{
    std::string::size_type pos = s.find(c);
    return (pos == std::string::npos) ? 0 : (1 + f_r(s.substr(pos+1), c));
}

#pragma mark
#pragma mark main

RogueScene::RogueScene()
:rogue_play_data_(),
base_map_size_(Size::ZERO),
base_tile_size_(Size::ZERO),
base_content_size_(Size::ZERO)
{
    CCLOG("new rogueScene");
}

RogueScene::~RogueScene()
{
    CCLOG("death rogueScene");
}

Scene* RogueScene::scene(int questId)
{
    Scene *scene = Scene::create();
    RogueScene *layer = RogueScene::createWithQuestId(questId);
    scene->addChild(layer);
    return scene;
}

RogueScene* RogueScene::createWithQuestId(int questId)
{
    auto *pRet = new RogueScene();
    if (pRet && pRet->initWithQuestId(questId))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool RogueScene::initWithQuestId(int questId)
{
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    // 乱数初期化
    srand((unsigned int)time(NULL));

    // データロード
    AccountData::getInstance()->load();
    
    ActorSprite::ActorDto actorDto;
    // 不一致の場合初期化
    if (AccountData::getInstance()->rogue_play_data_.quest_id != questId)
    {
        AccountData::getInstance()->reset();
        rogue_play_data_.quest_id = questId;
        
        actorDto.name = "ジニー";
        actorDto.faceImgId = 0;
        actorDto.imageResId = 1015;
        // 基本
        actorDto.attackRange = 1;
        actorDto.movePoint = 5;
        actorDto.playerId = 4; // actorId
        // 攻守
        actorDto.attackPoint = 5;
        actorDto.defencePoint = 1;
        // 経験値
        actorDto.exp = 0;
        actorDto.nextExp = 10;
        // HP
        actorDto.hitPoint = 15;
        actorDto.hitPointLimit = 15;
        actorDto.lv = 1;
        // 満腹度？精神力？
        actorDto.magicPoint = 100;
        actorDto.magicPointLimit = 100;
        // 装備
        actorDto.equip = ActorSprite::createEquipDto();
    }
    else
    {
        // ロード処理
        rogue_play_data_ = AccountData::getInstance()->rogue_play_data_;
        actorDto = AccountData::getInstance()->player_actor_;
    }
    
    // TouchEvent settings
    auto listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(RogueScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RogueScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RogueScene::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    auto winSize = Director::getInstance()->getWinSize();
    
    // ---------------------
    // タイルマップを生成
    // ---------------------
    // TODO: (kyokomi)ランダムなマップIDを指定する
    rogue_play_data_.floor_id = 2;
    
    auto pTiledMap = TMXTiledMap::create(StringUtils::format("tmx/quest_%d.tmx", rogue_play_data_.floor_id));

    pTiledMap->setPosition(Point::ZERO);
    this->addChild(pTiledMap, RogueScene::TiledMapLayerZOrder, RogueScene::TiledMapLayerTag);
    
    base_map_size_ = pTiledMap->getMapSize();
    base_tile_size_ = pTiledMap->getTileSize();
    base_content_size_ = pTiledMap->getContentSize();
    
    MapManager::getInstance()->initMapping(0, (int)base_map_size_.height, 0, (int)base_map_size_.width);

    // フロントレイヤー
    auto pFrontLayer = Layer::create();
    pTiledMap->addChild(pFrontLayer,
                        TiledMapIndexs::TiledMapFrontZOrder,
                        TiledMapTags::TiledMapFrontLayerTag);
    
    // エネミーレイヤー
    auto pEnemyLayer = Layer::create();
    pTiledMap->addChild(pEnemyLayer,
                        TiledMapIndexs::TiledMapEnemyBaseZOrder,
                        TiledMapTags::TiledMapEnemyBaseTag);
    
    // ドロップアイテムレイヤー
    auto pDropItemLayer = Layer::create();
    pTiledMap->addChild(pDropItemLayer,
                        TiledMapIndexs::TiledMapDropItemBaseZOrder,
                        TiledMapTags::TiledMapDropItemBaseTag);
    
    // ---------------------
    // グリッド線を生成
    // ---------------------
    // TODO: (kyokomi)一旦gridは非表示 オプション機能にする？
#if 0
    auto pGrid = createGridDraw();
    // マップに追加
    pTiledMap->addChild(pGrid, RogueScene::TiledMapIndexs::zGridLineIndex, RogueScene::TiledMapLayerTag::kGridLineTag);
#endif
    
    //-------------------------
    // ステータスバー？
    //-------------------------
    auto statusLayer = LayerColor::create(Color4B::BLACK);
    statusLayer->setContentSize(Size(winSize.width, base_tile_size_.height * 0.8));
    statusLayer->setPosition(Point(0, winSize.height - statusLayer->getContentSize().height));
    
    // 更新する
    auto sampleText = LabelTTF::create(" --F Lv-- HP ---/--- 満腹度 ---/---          - G", GAME_FONT(32), GAME_FONT_SIZE(32));
    
    sampleText->setPosition(Point(sampleText->getContentSize().width / 2, statusLayer->getContentSize().height / 2));
    statusLayer->addChild(sampleText);
    
    this->addChild(statusLayer, RogueScene::StatusBarLayerZOrder, RogueScene::StatusBarLayerTag);
    
    //-------------------------
    // ゲームログ表示
    //-------------------------
    auto pGameLogLayer = LayerColor::create(Color4B(0, 0, 0, 192));
    pGameLogLayer->setContentSize(Size(winSize.width * 0.5, base_tile_size_.height * 1.5));
    pGameLogLayer->setPosition(winSize.width / 2 - pGameLogLayer->getContentSize().width / 2, 0);
    
    CommonWindowUtil::attachWindowWaku(pGameLogLayer);
    
    int baseFontSize = 20;
    auto pLogTextLabel = LabelTTF::create("", GAME_FONT(baseFontSize), GAME_FONT_SIZE(baseFontSize), Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
    pLogTextLabel->setPosition(Point(pLogTextLabel->getContentSize().width / 2 + pLogTextLabel->getFontSize() / 4, pGameLogLayer->getContentSize().height - pLogTextLabel->getContentSize().height / 2 - pLogTextLabel->getFontSize() / 4));
    pGameLogLayer->addChild(pLogTextLabel);
    
    this->addChild(pGameLogLayer, RogueScene::GameLogLayerZOrder, RogueScene::GameLogLayerTag);
    
    // ------------------------
    // ミニマップ
    // ------------------------
    // 透明
    auto miniMapLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    // 1/8サイズ
    miniMapLayer->setContentSize(Size(base_map_size_.width * base_tile_size_.width / MINI_MAP_SCALE,
                                      base_map_size_.height * base_tile_size_.height / MINI_MAP_SCALE));
    // ステータスバーの下くらい
    miniMapLayer->setPosition(0, miniMapLayer->getPositionY() + winSize.height - miniMapLayer->getContentSize().height - statusLayer->getContentSize().height);
    this->addChild(miniMapLayer, RogueScene::MiniMapLayerZOrder, RogueScene::MiniMapLayerTag);
    
    // ------------------------
    // 障害物
    // ------------------------
    
    // 障害物をmapManagerに適応する
    auto pColisionLayer = pTiledMap->getLayer("colision");
    for (int x = 0; x < base_map_size_.width; x++)
    {
        for (int y = 0; y < base_map_size_.height; y++)
        {
            MapIndex mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
            auto tileMapIndex = mapIndexToTileIndex(mapIndex);
            if (pColisionLayer->getTileAt(Point(x, y)))
            {
                MapManager::getInstance()->addObstacle(&tileMapIndex);
            }
            else
            {
                // ミニマップ更新
                int tag = 10000 * x + 100 * y;
                addMiniMapItem(MapManager::getInstance()->getMapItem(&tileMapIndex), tag);
            }
        }
    }
    auto pGridBatchNode = getGridSpriteBatchNode();
    for (auto pNode : pGridBatchNode->getChildren())
    {
        if (pNode->getZOrder() == RogueScene::MiniMapLayerMapNoneZOrder)
        {
            pNode->setVisible(false);
        }
    }
    
    // TODO: (kyokomi)とりあえず。ちゃんと設定からとれるようにする
    int floorCount = 6;
    for (int i = 0; i < floorCount; i++)
    {
        auto pFloorLayer = pTiledMap->getLayer(StringUtils::format("floor_%d", i + 1));
        if (pFloorLayer)
        {
            pFloorLayer->setVisible(false);
        }
    }
    
    // ------------------------
    // イベントリ作成
    // ------------------------
    showItemList(1);
    hideItemList();
    // TODO: アイテム引き継ぎ
    getItemWindowLayer()->setItemList(AccountData::getInstance()->item_list_);
    getItemWindowLayer()->sortItemList();
    
    // ---------------------
    // プレイヤー生成
    // ---------------------
    ActorMapItem actorMapItem;
    actorMapItem.mapDataType = MapDataType::PLAYER;
    // 画面の中心（固定）
    MapIndex playerRandMapIndex = getRandomMapIndex(false, true);
    playerRandMapIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
    actorMapItem.mapIndex = playerRandMapIndex;
    actorMapItem.seqNo = 1;
    actorMapItem.moveDist = actorDto.movePoint;
    actorMapItem.attackDist = actorDto.attackRange;
    actorMapItem.moveDone = false;
    actorMapItem.attackDone = false;
    
    auto actorSprite = ActorSprite::createWithActorDto(actorDto);
    MapIndex baseActorIndex = pointToIndex(Point(winSize.width/2, winSize.height/2));
    actorSprite->setPosition(indexToPoint(baseActorIndex));
    actorSprite->setActorMapItem(actorMapItem);
    actorSprite->runBottomAction();
    // プレイヤーは画面中心にくるのでmapLayerに追加しない
    this->addChild(actorSprite, RogueScene::ActorPlayerZOrder, (RogueScene::ActorPlayerTag + actorMapItem.seqNo));

    // マップに追加
    MapManager::getInstance()->addActor(actorSprite->getActorMapItem());

    refreshStatus();
    
    // プレイヤーの位置表示用（同じく1/8サイズ）
    addMiniMapItem(actorSprite->getActorMapItem(), actorSprite->getTag());
    
    // プレイヤー位置にマップを移動
    MapIndex moveIndex = {
        playerRandMapIndex.x - baseActorIndex.x,
        playerRandMapIndex.y - baseActorIndex.y,
        actorMapItem.mapIndex.moveDictType
    };
    // 移動する距離をPointに変換
    auto addMovePoint = Point(base_tile_size_.width * moveIndex.x, base_tile_size_.height * moveIndex.y);
    // 移動
    pTiledMap->runAction(MoveTo::create(0.0f, pTiledMap->getPosition() - addMovePoint));
    
    // ---------------------
    // 敵キャラ生成
    // ---------------------
    ActorSprite::ActorDto enemyDto;
    enemyDto.name = "スライム";
    enemyDto.faceImgId = 0;
    enemyDto.imageResId = 1011;
    // 基本
    enemyDto.attackRange = 1; // 未使用
    enemyDto.movePoint = 10; // 索敵範囲
    enemyDto.playerId = 901;
    // 攻守
    enemyDto.attackPoint = 2;
    enemyDto.defencePoint = 0;
    // 経験値
    enemyDto.exp = 2;
    enemyDto.nextExp = 10;
    // HP
    enemyDto.hitPoint = 10;
    enemyDto.hitPointLimit = 10;
    enemyDto.lv = 1;
    // 満腹度？精神力？
    enemyDto.magicPoint = 100;
    enemyDto.magicPointLimit = 100;
    // 装備
    enemyDto.equip = ActorSprite::createEquipDto();
    
    MapIndex enemyMapIndex1 = getRandomMapIndex(false, true);
    tileSetEnemyActorMapItem(enemyDto, enemyMapIndex1);
    
    ActorSprite::ActorDto enemyDto2 = enemyDto;
    MapIndex enemyMapIndex2 = getRandomMapIndex(false, true);
    tileSetEnemyActorMapItem(enemyDto2, enemyMapIndex2);
    
    ActorSprite::ActorDto enemyDto3 = enemyDto;
    MapIndex enemyMapIndex3 = getRandomMapIndex(false, true);
    tileSetEnemyActorMapItem(enemyDto3, enemyMapIndex3);
    
    //-------------------------
    // アイテム配置
    //-------------------------
    int object_count = AccountData::getInstance()->rogue_play_data_.item_count;
    if (object_count < 0)
    {
        object_count = 0;
    }
    // TODO: (kyokomi) とりあえずアイテムとか武器はテストのため全種類配置してる
    for (int i = 0; i < 6; i++)
    {
        MUseItem mUseItem = MUseItemDao::getInstance()->selectById(i + 1);
        DropItemSprite::DropItemDto dropItemDto;
        dropItemDto.objectId = object_count + i + 1; // 単純に連番でいい
        dropItemDto.itemId = mUseItem.getUseItemId();
        dropItemDto.itemType = mUseItem.getUseItemType();
        dropItemDto.imageResId = mUseItem.getUseItemImageId();
        dropItemDto.name = mUseItem.getUseItemName();
        dropItemDto.isEquip = false;
        
        MapIndex mapIndex = getRandomMapIndex(false, false);
        tileSetDropMapItem(dropItemDto, mapIndex);

        object_count++;
    }

    for (int i = 0; i < 11; i++)
    {
        MWeapon mWeapon = MWeaponDao::getInstance()->selectById(i + 1);
        DropItemSprite::DropItemDto dropItemDto3;
        dropItemDto3.objectId = object_count + i + 1;
        dropItemDto3.itemId = mWeapon.getWeaponId(); // weaponId
        dropItemDto3.itemType = MUseItem::ItemType::EQUIP_WEAPON;
        dropItemDto3.imageResId = mWeapon.getWeaponImageId();
        dropItemDto3.name = mWeapon.getWeaponName();
        dropItemDto3.isEquip = false;
        
        MapIndex mapIndex3 = getRandomMapIndex(false, false);;
        tileSetDropMapItem(dropItemDto3, mapIndex3);
        
        object_count++;
    }
    for (int i = 0; i < 7; i++)
    {
        MAccessory mAccessory = MAccessoryDao::getInstance()->selectById(i + 1);
        
        DropItemSprite::DropItemDto dropItemDto4;
        dropItemDto4.objectId = object_count + i + 1;
        dropItemDto4.itemId = mAccessory.getAccessoryId();
        dropItemDto4.itemType = MUseItem::ItemType::EQUIP_ACCESSORY;
        dropItemDto4.imageResId = mAccessory.getAccessoryImageId();
        dropItemDto4.name = mAccessory.getAccessoryName();
        dropItemDto4.isEquip = false;
        
        MapIndex mapIndex4 = getRandomMapIndex(false, false);;
        tileSetDropMapItem(dropItemDto4, mapIndex4);
        
        object_count++;
    }
    
    // -------------------------------
    // 階段配置
    // -------------------------------
    
    MapIndex kaidanIndex = getRandomMapIndex(false, false);
    auto pKaidanSprite = Sprite::create("kaidan.png");
    pKaidanSprite->setPosition(indexToPoint(kaidanIndex));
    pKaidanSprite->setVisible(false);
    pTiledMap->addChild(pKaidanSprite, TiledMapIndexs::TiledMapObjectZOrder, TiledMapTags::TiledMapObjectTag);
    
    // マップに追加
    DropMapItem kaidanMapItem;
    kaidanMapItem.seqNo = 1;
    kaidanMapItem.mapIndex = kaidanIndex;
    kaidanMapItem.moveDist = 0;
    kaidanMapItem.attackDist = 0;
    kaidanMapItem.mapDataType = MapDataType::KAIDAN;
    MapManager::getInstance()->addDropItem(&kaidanMapItem);
    
    // ミニマップも更新
    addMiniMapItem(&kaidanMapItem, TiledMapTags::TiledMapObjectTag);
    
    // -------------------------------
    // メニュー
    // -------------------------------
    
    // keypad
    auto keypadMenuArray = createKeypadMenuItemArray();
    auto pMenu = Menu::createWithArray(keypadMenuArray);
    pMenu->setPosition(Point::ZERO);
    this->addChild(pMenu, RogueScene::MenuLayerZOrder, RogueScene::KeypadMenuTag);

    auto buttondMenuArray = createButtonMenuItemArray();
    auto pMenuButton = Menu::createWithArray(buttondMenuArray);
    pMenuButton->setPosition(Point::ZERO);
    this->addChild(pMenuButton, RogueScene::MenuLayerZOrder, RogueScene::ButtonMenuTag);
    
    // ---------------------------------
    // プレイヤーの先行
    changeGameStatus(GameStatus::PLAYER_TURN);
    
    //----------------------------------
    // 照明
    tiledMapLighting();

    return true;
}

DrawNode* RogueScene::createGridDraw()
{
    auto draw = DrawNode::create();
    draw->setPosition(Point::ZERO);

    // 線の太さと色
    float lineSize = 1 * 0.5;
    auto color = Color4F(1, 1, 1, 0.5f);

    // 縦線を引く
    for (int x = 1; x < base_map_size_.width; x++)
    {
        float xPoint = x * base_tile_size_.width;
        draw->drawSegment(Point(xPoint, 0), Point(xPoint, base_content_size_.height), lineSize, color);
    }
    // 横線を引く
    for (int y = 1; y < base_map_size_.height; y++)
    {
        float yPoint = y * base_tile_size_.height;
        draw->drawSegment(Point(0, yPoint), Point(base_content_size_.width, yPoint), lineSize, color);
    }

    return draw;
}

MenuItem* RogueScene::createKeypadMenuItemSprite(SpriteFrame* pBaseSpriteFrame, SpriteFrame* pBasePressSpriteFrame, const ccMenuCallback& callback)
{
    auto pKeypad = Sprite::createWithSpriteFrame(pBaseSpriteFrame);
    auto pKeypadPress = Sprite::createWithSpriteFrame(pBasePressSpriteFrame);
    
    pKeypadPress->setColor(Color3B::GRAY);
    auto pMenuKey = MenuItemSprite::create(pKeypad, pKeypadPress, callback);
    pMenuKey->setOpacity(128);
    return pMenuKey;
}

Vector<MenuItem*> RogueScene::createKeypadMenuItemArray()
{
    Vector<MenuItem*> resultArray;
    
    auto pKeyBase = Sprite::create("ui/keypad.png");
    auto pKeyBasePress = Sprite::create("ui/keypad_press.png");
    
    auto pMenuKeyUp = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyUpが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = this->pointToIndex(point);
            this->touchEventExec(this->indexToPoint(mapIndex.x, mapIndex.y + 1));
        }
    });
    
    pMenuKeyUp->setPosition(indexToPoint(1, 2));
    resultArray.pushBack(pMenuKeyUp);
    
    auto pMenuKeyRight = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyRightが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = this->pointToIndex(point);
            this->touchEventExec(this->indexToPoint(mapIndex.x + 1, mapIndex.y));
        }
    });
    pMenuKeyRight->setRotation(90.0f);
    pMenuKeyRight->setPosition(indexToPoint(2, 1));
    resultArray.pushBack(pMenuKeyRight);
    
    auto pMenuKeyDown = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyDownが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = this->pointToIndex(point);
            this->touchEventExec(this->indexToPoint(mapIndex.x, mapIndex.y - 1));
        }
    });
    pMenuKeyDown->setRotation(180.0f);
    pMenuKeyDown->setPosition(indexToPoint(1, 0));
    resultArray.pushBack(pMenuKeyDown);
    
    auto pMenuKeyLeft = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyLeftが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = this->pointToIndex(point);
            this->touchEventExec(this->indexToPoint(mapIndex.x - 1, mapIndex.y));
        }
    });
    pMenuKeyLeft->setRotation(270.0f);
    pMenuKeyLeft->setPosition(indexToPoint(0, 1));
    resultArray.pushBack(pMenuKeyLeft);
 
    return resultArray;
}

Vector<MenuItem*> RogueScene::createButtonMenuItemArray()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    Vector<MenuItem*> resultArray;
    
    auto a_button = Sprite::create("ui/a_button.png");
    auto a_buttonPress = Sprite::create("ui/a_button_press.png");
    a_buttonPress->setOpacity(128);
    auto pA_MenuButton = MenuItemSprite::create(a_button, a_buttonPress, [this](Ref* pSender) {
        CCLOG("Aボタンが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            this->attack();
        }
    });
    pA_MenuButton->setPosition(Point(win_size.width - base_tile_size_.width, indexToPoint(12, 1).y));
    pA_MenuButton->setTag(RogueScene::A_ButtonMenuTag);
    resultArray.pushBack(pA_MenuButton);
    
    auto b_button = Sprite::create("ui/b_button.png");
    auto b_buttonPress = Sprite::create("ui/b_button_press.png");
    b_buttonPress->setOpacity(128);
    auto pB_MenuButton = MenuItemSprite::create(b_button, b_buttonPress, [this](Ref* pSender) {
        CCLOG("Bボタンが押された！");
    });
    pB_MenuButton->setPosition(Point(win_size.width - base_tile_size_.width * 2, indexToPoint(11, 0).y));
    pB_MenuButton->setTag(RogueScene::B_ButtonMenuTag);
    resultArray.pushBack(pB_MenuButton);
    
    auto c_button = Sprite::create("ui/c_button.png");
    auto c_buttonPress = Sprite::create("ui/c_button_press.png");
    c_buttonPress->setOpacity(128);
    auto pC_MenuButton = MenuItemSprite::create(c_button, c_buttonPress, [this](Ref* pSender) {
        CCLOG("Cボタンが押された！");
        
        this->showCommonWindow("階段です。\n　\n次の階に進みますか？", [this](Ref* pSender){
            // OK
            this->hideCommonWindow();
            
            // 画面遷移
            this->changeScene(RogueScene::scene(rogue_play_data_.quest_id + 1));
            
        }, [this](Ref* pSender){
            // NG
            this->hideCommonWindow();
        });
        
    });
    pC_MenuButton->setPosition(Point(win_size.width - base_tile_size_.width * 3, indexToPoint(10, 1).y));
    pC_MenuButton->setTag(RogueScene::C_ButtonMenuTag);
    resultArray.pushBack(pC_MenuButton);
    
    auto d_button = Sprite::create("ui/d_button.png");
    auto d_buttonPress = Sprite::create("ui/d_button_press.png");
    d_buttonPress->setOpacity(128);
    auto pD_MenuButton = MenuItemSprite::create(d_button, d_buttonPress, [this](Ref* pSender) {
        CCLOG("Dボタンが押された！");
        showItemList(1);
    });
    pD_MenuButton->setPosition(Point(win_size.width - base_tile_size_.width * 2, indexToPoint(11, 2).y));
    pD_MenuButton->setTag(RogueScene::D_ButtonMenuTag);
    resultArray.pushBack(pD_MenuButton);
    
    return resultArray;
}

void RogueScene::onEnter() {
    CCLOG("%s(%d) onEnter", __FILE__, __LINE__);
    Scene::onEnter();
}

void RogueScene::onEnterTransitionDidFinish() {
    CCLOG("%s(%d) onEnterTransitionDidFinish", __FILE__, __LINE__);
    Scene::onEnterTransitionDidFinish();
}

#pragma mark
#pragma mark ゲームステート関連

void RogueScene::changeGameStatus(GameStatus gameStatus)
{
    CCLOG("turn %d change gameStatus %d => %d", rogue_play_data_.turn_count, rogue_play_data_.game_status, gameStatus);
    
    GameStatus beforeGameStatus = rogue_play_data_.game_status;
    rogue_play_data_.game_status = gameStatus;
    
    if (rogue_play_data_.game_status == GameStatus::GAME_OVER)
    {        
        // TODO: ゲームオーバーの演出
        
        // TODO: ゲームオーバー画面Scene？表示
        
        // TODO: とりあえずタイトルへ
        this->changeScene(TitleSceneLoader::scene());
    }
    else if ((beforeGameStatus == GameStatus::PLAYER_TURN || beforeGameStatus == GameStatus::PLAYER_ACTION || beforeGameStatus == GameStatus::PLAYER_NO_ACTION)
        && rogue_play_data_.game_status == GameStatus::ENEMY_TURN)
    {
        // 敵のターン開始時
        enemyTurn();
    }
    else if (rogue_play_data_.game_status == GameStatus::PLAYER_ACTION)
    {
        rogue_play_data_.no_action_count = 0;
    }
    else if (rogue_play_data_.game_status == GameStatus::PLAYER_NO_ACTION)
    {
        rogue_play_data_.no_action_count++;
    }
    else if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
    {
        auto pPlayer = getPlayerActorSprite(1);
        
        // カーソルはクリアする
        MapManager::getInstance()->clearCursor();
        // ターン数を進める
        rogue_play_data_.turn_count++;
        
        // TODO: とりあえずここで・・・
        auto pPlayerDto = pPlayer->getActorDto();
        
        // 10ターンに1空腹度が減るという
        if (rogue_play_data_.turn_count % 10 == 0)
        {
            if (pPlayerDto->magicPoint > 0)
            {
                pPlayerDto->magicPoint--;
            }
        }
        // 無行動が4ターン続くとHPが回復
        if (rogue_play_data_.no_action_count == 4)
        {
            rogue_play_data_.no_action_count = 0;
            
            if (pPlayerDto->hitPointLimit > pPlayerDto->hitPoint)
            {
                pPlayerDto->hitPoint++;
            }
        }
        
        // 敵のリポップ
        
        // ランダムなタイミングでランダムに湧く
        int rand = GetRandom(1, 10); // 1%
        if (rand == 1)
        {
            MapIndex rePopIndex = getRandomMapIndex(false, true);
            
            // TODO: 敵データは仮
            ActorSprite::ActorDto enemyDto;
            enemyDto.name = "スライム";
            enemyDto.faceImgId = 0;
            enemyDto.imageResId = 1011;
            // 基本
            enemyDto.attackRange = 1; // 未使用
            enemyDto.movePoint = 10; // 索敵範囲
            enemyDto.playerId = 901;
            // 攻守
            enemyDto.attackPoint = 2;
            enemyDto.defencePoint = 0;
            // 経験値
            enemyDto.exp = 2;
            enemyDto.nextExp = 10;
            // HP
            enemyDto.hitPoint = 10;
            enemyDto.hitPointLimit = 10;
            enemyDto.lv = 1;
            // 満腹度？精神力？
            enemyDto.magicPoint = 100;
            enemyDto.magicPointLimit = 100;
            // 装備
            enemyDto.equip = ActorSprite::createEquipDto();
            
            rePopIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
            tileSetEnemyActorMapItem(enemyDto, rePopIndex);
        }
    }
    
    // プレイステータス更新
    refreshStatus();
    // 照明情報更新
    tiledMapLighting();
}

void RogueScene::changeScene(Scene* scene)
{
    // TODO: とりあえず真っ黒のレイヤーを乗せる
    this->addChild(LayerColor::create(Color4B::BLACK), 99999999, 99999999);
    
    auto trans = TransitionProgressOutIn::create(1, scene);
    Director::getInstance()->replaceScene(trans);
}

float RogueScene::getAnimationSpeed()
{
    auto pMenu = getChildByTag(RogueScene::ButtonMenuTag);
    if (pMenu)
    {
        auto pB_ButtonMenuItem = static_cast<MenuItem*>(pMenu->getChildByTag(RogueScene::B_ButtonMenuTag));
        if (pB_ButtonMenuItem && pB_ButtonMenuItem->isSelected())
        {
            return 0.0f;
        }
    }
    
    return 0.2f;
}

MapIndex RogueScene::getRandomMapIndex(bool isColision, bool isActor)
{
    MapIndex randMapIndex;
    int randX = 0;
    int randY = 0;
    while (true)
    {
        // 出現位置をランダムで決める
        randX = GetRandom(0, base_map_size_.width-1); // x軸
        randY = GetRandom(0, base_map_size_.height-1); // y軸
        randMapIndex = {randX, randY, MoveDirectionType::MOVE_NONE};
        
        // 壁以外
        if (!isColision)
        {
            if (isTiledMapColisionLayer(randMapIndex))
            {
                // リトライ
                continue;
            }
        }
        
        // マップ上を確認
        auto randTargetMapItem = MapManager::getInstance()->getMapItem(&randMapIndex);
        
        if (isActor)
        {
            // アイテムの上もOK
            if (randTargetMapItem->mapDataType == MapDataType::NONE || randTargetMapItem->mapDataType == MapDataType::MAP_ITEM)
            {
                // OK
                break;
            }
        }
        else
        {
            // NONEのみ
            if (randTargetMapItem->mapDataType == MapDataType::NONE)
            {
                // OK
                break;
            }
        }
    }
    return randMapIndex;
}

#pragma mark
#pragma mark モンスターのAI関連

void RogueScene::enemyTurn()
{
    // モンスターの数だけ繰り返す
    std::list<ActorMapItem> enemyList = MapManager::getInstance()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList)
    {
        // ランダムでとどまるか移動するかきめる
        int rand = GetRandom(2, 2);
        if (rand == 1)
        {
            auto pEnemySprite = getEnemyActorSprite(enemyMapItem.seqNo);
            pEnemySprite->getActorMapItem()->moveDone = true;
            
            // とどまる
            logMessage("様子を見ている seqNo = %d", enemyMapItem.seqNo);
        }
        else if (rand == 2)
        {
            // プレイヤーに向かって移動 or プレイヤーに攻撃
            auto pPlayerActorSprite = getPlayerActorSprite(1);
            
            // プレイヤーの周辺で最もコストが低い箇所へ移動
            auto playerMapIndex = pPlayerActorSprite->getActorMapItem()->mapIndex;
            std::list<MapIndex> searchMapIndexList = MapManager::createRelatedMapIndexList(playerMapIndex);
            
            // そもそもプレイヤーが隣接しているかチェック
            bool isPlayerAttack = false;
            {
                for (MapIndex mapIndex : searchMapIndexList)
                {
                    if (MAP_INDEX_DIFF(enemyMapItem.mapIndex, mapIndex))
                    {
                        isPlayerAttack = true;
                    }
                }
            }
            
            MapIndex moveMapIndex = enemyMapItem.mapIndex;
            if (isPlayerAttack)
            {
                // 攻撃
                moveMapIndex =pPlayerActorSprite->getActorMapItem()->mapIndex;
            }
            else
            {
                // 移動可能な経路情報を設定
                MapManager::getInstance()->createActorFindDist(enemyMapItem.mapIndex, enemyMapItem.moveDist);
                
                // 最も移動コストがかからない場所を抽出
                MapItem targetMoveDistMapItem = MapManager::getInstance()->searchTargetMapItem(searchMapIndexList);
                
                // 移動リスト作成
                if (targetMoveDistMapItem.mapDataType == MapDataType::MOVE_DIST)
                {
                    std::list<MapIndex> moveList = MapManager::getInstance()->createMovePointList(targetMoveDistMapItem.mapIndex,
                                                                                    &enemyMapItem);
                    std::list<MapIndex>::iterator it = moveList.begin();
                    it++;
                    moveMapIndex = *it; // 2件目を取得(1件目は自分なので）
                    it = moveList.end();
                }
            }
            searchMapIndexList.clear();
            
            // 移動有無関係なく向きは変える
            auto pEnemySprite = getEnemyActorSprite(enemyMapItem.seqNo);
            MapIndex addMoveIndex = {
                moveMapIndex.x - pEnemySprite->getActorMapItem()->mapIndex.x,
                moveMapIndex.y - pEnemySprite->getActorMapItem()->mapIndex.y,
                MapManager::getInstance()->checkMoveDirectionType(moveMapIndex, pEnemySprite->getActorMapItem()->mapIndex)
            };
            pEnemySprite->runMoveAction(addMoveIndex);
            // 行動前にする
            pEnemySprite->getActorMapItem()->moveDone = false;
            pEnemySprite->getActorMapItem()->attackDone = false;
            
            if (isMapLayerOver(moveMapIndex))
            {
                CCLOG("移動不可 seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->getActorMapItem()->moveDone = true;
            }
            else if (isTiledMapColisionLayer(moveMapIndex))
            {
                logMessage("壁ドーン seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->getActorMapItem()->moveDone = true;
            }
            else if (MapManager::getInstance()->getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::ENEMY)
            {
                if (MAP_INDEX_DIFF(enemyMapItem.mapIndex, moveMapIndex))
                {
                    //logMessage("待機 seqNo = %d (%d, %d)");
                }
                else
                {
                    logMessage("敵ドーン seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                }
                pEnemySprite->getActorMapItem()->moveDone = true;
            }
            else if (MapManager::getInstance()->getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::PLAYER)
            {
                // 移動中のステータスへ
                changeGameStatus(GameStatus::ENEMY_ACTION);
                
                // 攻撃アニメーション
                float speed = getAnimationSpeed();
                auto pMove1 = MoveTo::create(speed, pEnemySprite->getPosition() + indexToPointNotTileSize(addMoveIndex));
                auto pMove2 = MoveTo::create(speed, pEnemySprite->getPosition());
                
                pEnemySprite->runAction(Sequence::create(pMove1, pMove2,
                                                         CallFunc::create([this, pEnemySprite, pPlayerActorSprite]() {
                    auto pPlayerDto = pPlayerActorSprite->getActorDto();
                    auto pEnemyDto = pEnemySprite->getActorDto();
                                        
                    int damage = BattleLogic::exec(pEnemyDto, pPlayerDto);
                    
                    // 攻撃イベント
                    this->logMessage("%sの攻撃: %sに%dダメージ", pEnemyDto->name.c_str(), pPlayerDto->name.c_str(), damage);
                    
                    // プレイヤーステータス更新と死亡判定
                    this->refreshStatus();
                    
                    pEnemySprite->getActorMapItem()->attackDone = true;
                    
                    checkEnmeyTurnEnd();
                    
                }), NULL));
            }
            else
            {
                // アニメーション中のステータスへ
                changeGameStatus(GameStatus::ENEMY_ACTION);
                // 移動開始
                moveMap(addMoveIndex, enemyMapItem.seqNo, enemyMapItem.mapDataType,
                        CallFunc::create([this, pEnemySprite]() {
                    // 移動終わり
                    pEnemySprite->getActorMapItem()->moveDone = true;
                    
                    checkEnmeyTurnEnd();
                }));
            }
        }
    }
    checkEnmeyTurnEnd();
}

void RogueScene::checkEnmeyTurnEnd()
{
    bool isTurnEnd = true;
    std::list<ActorMapItem> enemyList = MapManager::getInstance()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList)
    {
        auto pEnemySprite = getEnemyActorSprite(enemyMapItem.seqNo);
        auto pEnemyMapItem = pEnemySprite->getActorMapItem();
        if (!pEnemyMapItem->moveDone && !pEnemyMapItem->attackDone)
        {
            isTurnEnd = false;
            break;
        }
    }
    if (isTurnEnd)
    {
        changeGameStatus(GameStatus::PLAYER_TURN);
    }
    else
    {
        changeGameStatus(GameStatus::ENEMY_TURN);
    }
}

#pragma mark
#pragma mark タッチイベント関連

bool RogueScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    CCLOG("RogueScene onTouchBegan");
    if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
    {
        return true;
    }
    return false;
}

void RogueScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{

}

void RogueScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
    {
        // keypadにしました。
    }
}

void RogueScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unscheduleUpdate();
}


void RogueScene::touchEventExec(cocos2d::Point touchPoint)
{
    auto pMapLayer = (TMXTiledMap*)getChildByTag(TiledMapLayerTag);
    // マップ移動分を考慮
    MapIndex touchPointMapIndex = touchPointToIndex(touchPoint - pMapLayer->getPosition());
    CCLOG("touchEventExec touchPointMapIndex x = %d y = %d", touchPointMapIndex.x, touchPointMapIndex.y);
    
    // 画面外判定
    if (isMapLayerOver(touchPointMapIndex))
    {
        // タッチ判定とみなさない
        return;
    }
    // タッチした位置が有効なIndexか判定
    MapIndex addMoveIndex = checkTouchEventIndex(touchPointMapIndex);
    if (addMoveIndex.x == 0 && addMoveIndex.y == 0)
    {
        // タッチ判定とみなさない
        return;
    }
    
    touchEventExec(addMoveIndex, touchPointMapIndex);
}

void RogueScene::touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex)
{
    // キャラの向きを変更
    auto pActorSprite = getPlayerActorSprite(1);
    pActorSprite->runMoveAction(addMoveIndex);
    pActorSprite->getActorMapItem()->mapIndex.moveDictType = addMoveIndex.moveDictType;
    
    // 敵をタッチした
    auto pEnemyMapItem = MapManager::getInstance()->getActorMapItem(&touchPointMapIndex);
    if (pEnemyMapItem->mapDataType == MapDataType::ENEMY)
    {
        // 向きだけ変えてターン経過しない
    }
    else
    {
        // 障害物判定
        if (isTiledMapColisionLayer(touchPointMapIndex))
        {
            // TODO: ぶつかるSE再生
            logMessage("壁ドーン SE再生");
            
            // ターン経過なし
        }
        else
        {
            changeGameStatus(GameStatus::PLAYER_NO_ACTION);
            
            // アイテムに重なったときの拾う処理
            auto pTouchPointMapItem = MapManager::getInstance()->getMapItem(&touchPointMapIndex);
            if (pTouchPointMapItem->mapDataType == MapDataType::MAP_ITEM)
            {
                // ドロップアイテムを拾う
                auto pDropMapItem = static_cast<DropMapItem*>(pTouchPointMapItem);
                
                // TODO: 拾うSE再生
                
                // itemを取得
                auto pDropItemLayer = getChildByTag(RogueScene::TiledMapLayerTag)->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
                auto pDropItemSprite = static_cast<DropItemSprite*>(pDropItemLayer->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag + pDropMapItem->seqNo));
                
                // メッセージログ
                auto pDropItemDto = pDropItemSprite->getDropItemDto();
                logMessage("%sを拾った。", pDropItemDto->name.c_str());
                
                // イベントリに追加する
                getItemWindowLayer()->addItemList(*pDropItemDto);
                
                // Map上から削除する
                removeDropItemSprite(pDropItemLayer, pDropItemSprite);
            }
            else if (pTouchPointMapItem->mapDataType == MapDataType::KAIDAN)
            {
                // TODO: (kyokomi)階段SE
                
                // 階段下りる判定
                
                this->showCommonWindow("階段です。\n　\n次の階に進みますか？", [this, pActorSprite](Ref* pSender){
                    // OK
                    
                    // 閉じる
                    this->hideCommonWindow();
                    
                    // save
                    rogue_play_data_.quest_id += 1;
                    
                    AccountData::getInstance()->player_actor_ = *(pActorSprite->getActorDto());
                    AccountData::getInstance()->rogue_play_data_ =  rogue_play_data_;
                    // TODO: アイテムリスト暫定
                    AccountData::getInstance()->item_list_ = getItemWindowLayer()->getItemList();
                    AccountData::getInstance()->save();
                    
                    // 画面遷移
                    this->changeScene(RogueScene::scene(rogue_play_data_.quest_id));
                    
                }, [this](Ref* pSender){
                    // NG
                    
                    // 閉じる
                    this->hideCommonWindow();
                });
            }
            
            // 移動処理
            moveMap(addMoveIndex, pActorSprite->getActorMapItem()->seqNo, pActorSprite->getActorMapItem()->mapDataType, CallFunc::create([this](void) {
                
                // ターンエンド
                changeGameStatus(GameStatus::ENEMY_TURN);
            }));
            
            // コールバックまでgameStatusを更新はしない
        }
    }
}

void RogueScene::attack()
{
    auto pActorSprite = getPlayerActorSprite(1);
    
    MapIndex addMapIndex = {0, 0, MoveDirectionType::MOVE_NONE};
    MapIndex attackMapIndex = pActorSprite->getActorMapItem()->mapIndex;
    if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_UP)
    {
        // 上
        addMapIndex.y += 1;
        attackMapIndex.y += 1;
    }
    else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_RIGHT)
    {
        // 右
        addMapIndex.x += 1;
        attackMapIndex.x += 1;
    }
    else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_LEFT)
    {
        // 左
        addMapIndex.x -= 1;
        attackMapIndex.x -= 1;
    }
    else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_DOWN)
    {
        // 下
        addMapIndex.y -= 1;
        attackMapIndex.y -= 1;
    }
    
    changeGameStatus(GameStatus::PLAYER_ACTION);
    
    // 攻撃アニメーション
    float speed = getAnimationSpeed();
    auto pMove1 = MoveTo::create(speed, pActorSprite->getPosition() + indexToPointNotTileSize(addMapIndex.x, addMapIndex.y));
    auto pMove2 = MoveTo::create(speed, pActorSprite->getPosition());

    ActorSprite* pEnemySprite = NULL;
    // 敵をタッチした
    auto pEnemyMapItem = MapManager::getInstance()->getActorMapItem(&attackMapIndex);
    if (pEnemyMapItem->mapDataType == MapDataType::ENEMY)
    {
        pEnemySprite = getEnemyActorSprite(pEnemyMapItem->seqNo);
    }
    else
    {
        // 空振り
    }
    
    // アニメーション
    pActorSprite->runAction(Sequence::create(pMove1, pMove2, CallFunc::create([this, pActorSprite, pEnemySprite](void) {
        if (pEnemySprite)
        {
            auto pPlayerDto = pActorSprite->getActorDto();
            auto pEnemyDto = pEnemySprite->getActorDto();
            
            int damage = BattleLogic::exec(pPlayerDto, pEnemyDto);
            
            // 攻撃イベント
            logMessage("%sの攻撃: %sに%dのダメージ", pPlayerDto->name.c_str(), pEnemyDto->name.c_str(), damage);
            
            // 敵の死亡判定
            if (pEnemyDto->hitPoint == 0)
            {
                logMessage("%sを倒した。経験値%dを得た。", pEnemyDto->name.c_str(), pEnemyDto->exp);
                
                // 経験値更新
                pPlayerDto->exp += pEnemyDto->exp;
                if (MLevelDao::getInstance()->checkLevelUp(pPlayerDto->lv, pPlayerDto->exp))
                {
                    pPlayerDto->lv++;
                    auto mLevel = MLevelDao::getInstance()->selectById(pPlayerDto->lv);
                    pPlayerDto->hitPointLimit += mLevel.getGrowHitPoint();
                    
                    // TODO: レベルアップ演出（SE？）
                    
                    logMessage("%sはレベル%dになった。", pPlayerDto->name.c_str(), pPlayerDto->lv);
                }
                
                // マップから消える
                removeEnemyActorSprite(pEnemySprite);
            }
        }
        changeGameStatus(GameStatus::ENEMY_TURN);
        
    }), NULL));
}

MapIndex RogueScene::checkTouchEventIndex(MapIndex touchPointMapIndex)
{
    // 移動距離計算
    auto pActorSprite = getPlayerActorSprite(1);
    
    MapIndex addMoveIndex;
    addMoveIndex.x = touchPointMapIndex.x - pActorSprite->getActorMapItem()->mapIndex.x;
    addMoveIndex.y = touchPointMapIndex.y - pActorSprite->getActorMapItem()->mapIndex.y;
    
    if (addMoveIndex.x == 1 && addMoveIndex.y == 0)
    {
        addMoveIndex.moveDictType = MoveDirectionType::MOVE_RIGHT;
    }
    else if (addMoveIndex.x == -1 && addMoveIndex.y == 0)
    {
        addMoveIndex.moveDictType = MoveDirectionType::MOVE_LEFT;
    }
    else if (addMoveIndex.x == 0 && addMoveIndex.y == 1)
    {
        addMoveIndex.moveDictType = MoveDirectionType::MOVE_UP;
    }
    else if (addMoveIndex.x == 0 && addMoveIndex.y == -1)
    {
        addMoveIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
    }
    else
    {
        // 上記以外は有効なタッチじゃない
        addMoveIndex.x = 0;
        addMoveIndex.y = 0;
        addMoveIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
        return addMoveIndex;
    }
    
    // プレイヤーから1マス以内なら移動or攻撃と判断
    CCLOG("move ok %d,%d %d,%d", pActorSprite->getActorMapItem()->mapIndex.x, pActorSprite->getActorMapItem()->mapIndex.y, touchPointMapIndex.x, touchPointMapIndex.y);
    
    CCLOG("addMoveIndex %d,%d", addMoveIndex.x, addMoveIndex.y);
    return addMoveIndex;
}

#pragma mark
#pragma mark map関連

void RogueScene::moveMap(MapIndex addMoveIndex, int actorSeqNo, MapDataType mapDataType, CallFunc* moveFinishedCallFunc)
{
    ActorSprite* pActorSprite;
    if (mapDataType == MapDataType::PLAYER)
    {
        pActorSprite = getPlayerActorSprite(actorSeqNo);
    }
    else
    {
        pActorSprite = getEnemyActorSprite(actorSeqNo);
    }
    
    // 移動距離計算
    auto pMapLayer = (TMXTiledMap*)getChildByTag(TiledMapLayerTag);
    
    // 移動する距離をPointに変換
    auto addMovePoint = Point(base_tile_size_.width * addMoveIndex.x, base_tile_size_.height * addMoveIndex.y);
    
    Node* pActionTargetNode;
    FiniteTimeAction* pMoveRunAction;
    // マップを指定index分移動
    if (pActorSprite->getActorMapItem()->mapDataType == MapDataType::PLAYER)
    {
        // プレイヤーならマップが移動
        pMoveRunAction = MoveTo::create(getAnimationSpeed(), pMapLayer->getPosition() - addMovePoint);
        pActionTargetNode = pMapLayer;
    }
    else
    {
        // モンスターは普通にモンスターが移動
        pMoveRunAction = MoveTo::create(getAnimationSpeed(), pActorSprite->getPosition() + addMovePoint);
        pActionTargetNode = pActorSprite;
    }

    // マップ情報を更新
    MapIndex moveMapIndex;
    moveMapIndex.moveDictType = addMoveIndex.moveDictType;
    auto actorMapItem = pActorSprite->getActorMapItem();
    moveMapIndex.x = actorMapItem->mapIndex.x + addMoveIndex.x;
    moveMapIndex.y = actorMapItem->mapIndex.y + addMoveIndex.y;
    MapManager::getInstance()->moveActor(actorMapItem, moveMapIndex);
    // actor情報も更新
    actorMapItem->mapIndex = moveMapIndex;
    
    // ミニマップも更新
    auto pMiniMapLayer = getGridSpriteBatchNode();
    if (pMiniMapLayer)
    {
        auto pMiniMapActorNode = pMiniMapLayer->getChildByTag(pActorSprite->getTag());
        
        float scale = 1.0f / MINI_MAP_SCALE;
        auto point = indexToPointNotTileSize(pActorSprite->getActorMapItem()->mapIndex) / MINI_MAP_SCALE;
        pMiniMapActorNode->setPosition(point.x + pMiniMapActorNode->getContentSize().width / 2 * scale,
                                       point.y + pMiniMapActorNode->getContentSize().height / 2 * scale);
    }
    // move実行
    pActionTargetNode->runAction(Sequence::create(pMoveRunAction, moveFinishedCallFunc, NULL));
}

bool RogueScene::isTiledMapColisionLayer(MapIndex touchPointMapIndex)
{
    // 障害物判定
    auto pMapLayer = static_cast<TMXTiledMap*>(getChildByTag(TiledMapLayerTag));
    
    auto pColisionLayer = pMapLayer->getLayer("colision");
    // TileMapは左上から0,0なので座標変換する
    auto touchPointTileIndex = mapIndexToTileIndex(touchPointMapIndex);
    auto pTileSprite = pColisionLayer->getTileAt(Point(touchPointTileIndex.x, touchPointTileIndex.y));
    if (pTileSprite)
    {
        // 障害物なので移動とかできない
        CCLOG("colision touchPointTileIndex x = %d y = %d", touchPointTileIndex.x, touchPointTileIndex.y);
        return true;
    }
    
    return false;
}

bool RogueScene::isMapLayerOver(MapIndex touchPointMapIndex)
{
    if (base_map_size_.width <= touchPointMapIndex.x ||
        base_map_size_.height <= touchPointMapIndex.y ||
        0 > touchPointMapIndex.x ||
        0 > touchPointMapIndex.y)
    {
        return true;
    }
    return false;
}

#pragma mark
#pragma mark UI関連

void RogueScene::logMessage(const char * pszFormat, ...)
{
    va_list ap;
    va_start(ap, pszFormat);
    char buf[RogueScene::MAX_LOG_LENGTH];
    vsnprintf(buf, RogueScene::MAX_LOG_LENGTH-3, pszFormat, ap);
    va_end(ap);
    
    CCLOG("logMessage: %s", buf);
    
    auto pGameLogNode = getChildByTag(RogueScene::GameLogLayerTag);
    // とりあえず子要素がないなら無理
    if (!pGameLogNode || pGameLogNode->getChildrenCount() <= 0)
    {
        return;
    }
    
    auto pGameLogText = static_cast<LabelTTF*>(pGameLogNode->getChildren().at(1)); // TODO: 1子しかaddしてないから動く。ちゃんとしないと・・・
    if (pGameLogText)
    {
        // TODO: 別クラスにしてログをlistで保持する。デフォルトの表示は1件だけで、center寄せ表示でいいかと
        auto pMessage = String::create(buf);
        
        pMessage->append("\n");

        std::string nowString = pGameLogText->getString();
        
        int count = f_r(nowString, '\n');
        
        // 3行まで表示
        if (count >= 3)
        {
            int size = nowString.size();
            unsigned int loc = nowString.find_last_of('\n', size);
            CCLOG("logMessage: loc = %d size = %d", loc, size);
            if (loc != std::string::npos)
            {
                nowString.erase(loc, nowString.size() - loc);
            }
        }
        pMessage->append(nowString);
        pGameLogText->setString(pMessage->getCString());
        pGameLogText->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
        pGameLogText->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
        pGameLogText->setPosition(Point(pGameLogText->getFontSize() / 4 + pGameLogText->getContentSize().width / 2, pGameLogNode->getContentSize().height - pGameLogText->getContentSize().height / 2 - pGameLogText->getFontSize() / 4));
    }
}

void RogueScene::showItemList(int showTextIndex)
{
    // いまのところ使用してないindex
    if (showTextIndex <= 0)
    {
        return;
    }
    
    auto winSize = Director::getInstance()->getWinSize();
    
    auto pModalLayer = getModalLayer();
    pModalLayer->setVisible(true);
    
    auto pItemWindowLayer = getItemWindowLayer();
    if (pItemWindowLayer)
    {
        pItemWindowLayer->reloadItemList();
        pItemWindowLayer->setVisible(true);
    }
    else
    {
        // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
        pItemWindowLayer = ItemWindowLayer::createWithContentSize(winSize * 0.8);
        pItemWindowLayer->setPosition(Point(winSize.width / 2 - pItemWindowLayer->getContentSize().width / 2,
                                            winSize.height /2 - pItemWindowLayer->getContentSize().height / 2));
        pItemWindowLayer->setItemDropMenuCallback([this](Ref* pSender, DropItemSprite::DropItemDto dropItemDto) {
            CCLOG("RogueScene::itemDropMenuCallback");
            
            auto pPlayerSprite = getPlayerActorSprite(1);
            
            // すでにアイテムが置いてある場合は置けない
            if (MapManager::getInstance()->getDropMapItem(&pPlayerSprite->getActorMapItem()->mapIndex)->mapDataType != MapDataType::NONE)
            {
                this->logMessage("%sを床におけなかった。", dropItemDto.name.c_str());
                // アイテムを戻す
                this->getItemWindowLayer()->addItemList(dropItemDto);
            }
            else
            {
                // 装備してたら外す
                if (dropItemDto.isEquip)
                {
                    dropItemDto.isEquip = false;
                    
                    if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_WEAPON)
                    {
                        pPlayerSprite->equipReleaseWeapon();
                    }
                    else if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_ACCESSORY)
                    {
                        pPlayerSprite->equipReleaseAccessory();
                    }
                }
                
                // アイテムをマップのプレイヤーの足元に置く
                if (this->tileSetDropMapItem(dropItemDto, pPlayerSprite->getActorMapItem()->mapIndex))
                {
                    this->logMessage("%sを床においた。", dropItemDto.name.c_str());
                    
                    // ターン消費
                    this->changeGameStatus(RogueScene::ENEMY_TURN);
                }
            }
            
            // インベントリは閉じる
            this->hideItemList();
        });
        
        pItemWindowLayer->setItemUseMenuCallback([this](Ref* pSender, DropItemSprite::DropItemDto dropItemDto) {
            CCLOG("RogueScene::itemUseMenuCallback");
            
            auto pPlayerSprite = getPlayerActorSprite(1);
            
            // itemIdで処理してくれるlogicへ
            std::string useMessage = ItemLogic::use(dropItemDto.itemId, pPlayerSprite->getActorDto());
            
            this->logMessage(useMessage.c_str());
            
            // インベントリは閉じる
            this->hideItemList();
            
            // ターン消費
            this->changeGameStatus(RogueScene::ENEMY_TURN);
        });
        
        pItemWindowLayer->setItemEquipMenuCallback([this, pItemWindowLayer](Ref* pSender, DropItemSprite::DropItemDto dropItemDto) {
            CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", dropItemDto.itemType);
            
            auto pPlayerSprite = getPlayerActorSprite(1);
            
            if (dropItemDto.isEquip)
            {
                if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_WEAPON)
                {
                    // 解除
                    pItemWindowLayer->setItemEquip(pPlayerSprite->getActorDto()->equip.weaponObjectId, false);
                    // 武器装備
                    pPlayerSprite->getActorDto()->equip.weaponObjectId = dropItemDto.objectId;
                    pPlayerSprite->getActorDto()->equip.weaponId       = dropItemDto.itemId;
                    pPlayerSprite->getActorDto()->equip.weaponImgResId = dropItemDto.imageResId;
                    pPlayerSprite->getActorDto()->equip.weaponName     = dropItemDto.name;
                    pPlayerSprite->getActorDto()->equip.weaponStr      = 5; // TODO: valueはマスタから実はnameも取りたい・・・
                }
                else if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_ACCESSORY)
                {
                    // 解除
                    pItemWindowLayer->setItemEquip(pPlayerSprite->getActorDto()->equip.accessoryObjectId, false);
                    // 防具装備
                    pPlayerSprite->getActorDto()->equip.accessoryObjectId = dropItemDto.objectId;
                    pPlayerSprite->getActorDto()->equip.accessoryId       = dropItemDto.itemId;
                    pPlayerSprite->getActorDto()->equip.accessoryImgResId = dropItemDto.imageResId;
                    pPlayerSprite->getActorDto()->equip.accessoryName     = dropItemDto.name;
                    pPlayerSprite->getActorDto()->equip.accessoryDef      = 2; // TODO: valueはマスタから実はnameも取りたい・・・
                }
                this->logMessage("%sを装備した。", dropItemDto.name.c_str());
            }
            else
            {
                if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_WEAPON)
                {
                    // 武器解除
                    pPlayerSprite->equipReleaseWeapon();
                }
                else if (dropItemDto.itemType == MUseItem::ItemType::EQUIP_ACCESSORY)
                {
                    // 防具装備
                    pPlayerSprite->equipReleaseAccessory();
                }
                this->logMessage("%sの装備をはずした。", dropItemDto.name.c_str());
            }
            
            // インベントリは閉じる
            this->hideItemList();
            
            // ターン消費
            this->changeGameStatus(RogueScene::ENEMY_TURN);
        });
        
        pModalLayer->addChild(pItemWindowLayer, RogueScene::ItemListLayerZOrder, RogueScene::ItemListWindowTag);

        const int font_size = 20;
        
        // 並び替えボタン
        auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("並び替え", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), Size(12, 4), [this, pItemWindowLayer](Ref *pSender) {
            // 並び替え
            pItemWindowLayer->sortItemList();
            pItemWindowLayer->reloadItemList();
        });
        sort_menu_item_label->setPosition(Point(pItemWindowLayer->getContentSize().width, pItemWindowLayer->getContentSize().height + sort_menu_item_label->getContentSize().height / 2));
        
        // 閉じるボタン
        auto pCloseMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("とじる", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), Size(12, 4), [this](Ref *pSender) {
            // 閉じる
            this->hideItemList();
        });
        pCloseMenuItemLabel->setPosition(Point(pItemWindowLayer->getContentSize().width, pCloseMenuItemLabel->getPositionY() -  pCloseMenuItemLabel->getContentSize().height / 2));
        
        auto pMenu = Menu::create(sort_menu_item_label, pCloseMenuItemLabel, NULL);
        pMenu->setPosition(Point::ZERO);
        
        pItemWindowLayer->addChild(pMenu);
    }
}

void RogueScene::hideItemList()
{
    // モーダルレイヤー非表示にする
    auto pItemWindow = getItemWindowLayer();
    if (pItemWindow)
    {
        pItemWindow->setVisible(false);
    }
    
    auto pModalLayer = getModalLayer();
    if (pModalLayer)
    {
        pModalLayer->setVisible(false);
    }
}

void RogueScene::showCommonWindow(std::string titleText, const ccMenuCallback& okMenuItemCallback, const ccMenuCallback& ngMenuItemCallback)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    // モーダルレイヤー作成
    auto pModalLayer = getModalLayer();
    pModalLayer->setVisible(true);
    
    auto pCommonWindow = getCommonWindow();
    if (pCommonWindow)
    {
        pCommonWindow->setVisible(true);
    }
    else
    {
        // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
        pCommonWindow = AlertDialogLayer::createWithContentSize(winSize * 0.5, titleText, "は　い", "いいえ");
        pCommonWindow->setPosition(Point(winSize.width / 2 - pCommonWindow->getContentSize().width / 2,
                                         winSize.height /2 - pCommonWindow->getContentSize().height / 2));
        pModalLayer->addChild(pCommonWindow, RogueScene::CommonWindowZOrder, RogueScene::CommonWindowTag);
    }
    pCommonWindow->setTitleText(titleText);
    pCommonWindow->setOkMenuItemCallback(okMenuItemCallback);
    pCommonWindow->setNgMenuItemCallback(ngMenuItemCallback);
}

AlertDialogLayer* RogueScene::getCommonWindow()
{
    auto pModalLayer = getModalLayer();
    return static_cast<AlertDialogLayer*>(pModalLayer->getChildByTag(RogueScene::CommonWindowTag));
}

void RogueScene::hideCommonWindow()
{
    auto pCommonWindow = getCommonWindow();
    if (pCommonWindow)
    {
        pCommonWindow->setVisible(false);
    }
    
    auto pModalLayer = getModalLayer();
    if (pModalLayer)
    {
        pModalLayer->setVisible(false);
    }
}


#pragma mark
#pragma mark Tile関連

bool RogueScene::tileSetEnemyActorMapItem(ActorSprite::ActorDto enemyActorDto, MapIndex mapIndex)
{
    // すでにプレイヤーが置いてある場合は置けない
    if (MapManager::getInstance()->getActorMapItem(&mapIndex)->mapDataType != MapDataType::NONE)
    {
        return false;
    }
    auto pTileMapLayer = getChildByTag(RogueScene::TiledMapLayerTag);
    auto pEnemyLayer = pTileMapLayer->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    // 出現数のカウントをseqNoにする
    long enemyCount = rogue_play_data_.enemy_count;
    rogue_play_data_.enemy_count++;
    
    ActorMapItem enemyMapItem;
    enemyMapItem.mapDataType = MapDataType::ENEMY;
    enemyMapItem.mapIndex = mapIndex;
    enemyMapItem.seqNo = enemyCount;
    enemyMapItem.moveDist = enemyActorDto.movePoint;
    enemyMapItem.attackDist = enemyActorDto.attackRange;
    enemyMapItem.moveDone = false;
    enemyMapItem.attackDone = false;
    
    auto enemySprite = ActorSprite::createWithActorDto(enemyActorDto);
    enemySprite->setPosition(indexToPoint(enemyMapItem.mapIndex));
    enemySprite->setActorMapItem(enemyMapItem);
    enemySprite->runBottomAction();
    pEnemyLayer->addChild(enemySprite, TiledMapIndexs::TiledMapEnemyBaseZOrder, (TiledMapTags::TiledMapEnemyBaseTag + enemyMapItem.seqNo));
    
    // マップに追加
    MapManager::getInstance()->addActor(enemySprite->getActorMapItem());
    
    // ミニマップも更新
    addMiniMapItem(enemySprite->getActorMapItem(), enemySprite->getTag());
    
    return true;
}

void RogueScene::removeEnemyActorSprite(ActorSprite* pEnemySprite)
{
    auto pEnemyMapLayer = getChildByTag(RogueScene::TiledMapLayerTag)->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    
    // mapManagerから消す
    MapManager::getInstance()->removeMapItem(pEnemySprite->getActorMapItem());
    
    // ミニマップを更新
    getGridSpriteBatchNode()->removeChildByTag(pEnemySprite->getTag());
    
    // Map上からremoveする
    pEnemyMapLayer->removeChild(pEnemySprite);
}

bool RogueScene::tileSetDropMapItem(DropItemSprite::DropItemDto dropItemDto, MapIndex mapIndex)
{
    // すでにアイテムが置いてある場合は置けない
    if (MapManager::getInstance()->getDropMapItem(&mapIndex)->mapDataType != MapDataType::NONE)
    {
        return false;
    }
    
    auto pTileMapLayer = getChildByTag(RogueScene::TiledMapLayerTag);
    auto pDropItemLayer = pTileMapLayer->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
    
    DropMapItem dropMapItem;
    // 一意になるようにする x * 100 + y（100マスないからいけるはず）
    dropMapItem.seqNo = mapIndex.x * 100 + mapIndex.y;
    dropMapItem.itemId = dropItemDto.itemId;
    dropMapItem.mapDataType = MapDataType::MAP_ITEM;
    dropMapItem.moveDist = 0;
    dropMapItem.mapIndex = mapIndex;
    
    auto pDropItemSprite = DropItemSprite::createWithDropItemDto(dropItemDto);
    pDropItemSprite->setDropMapItem(dropMapItem);
    pDropItemSprite->setPosition(indexToPoint(dropMapItem.mapIndex));
    pDropItemLayer->addChild(pDropItemSprite, TiledMapIndexs::TiledMapDropItemBaseZOrder, TiledMapTags::TiledMapDropItemBaseTag + dropMapItem.seqNo);
    
    // マップに追加
    MapManager::getInstance()->addDropItem(pDropItemSprite->getDropMapItem());
    
    // ミニマップも更新
    addMiniMapItem(pDropItemSprite->getDropMapItem(), pDropItemSprite->getTag());
    
    return true;
}

void RogueScene::removeDropItemSprite(Node* pRemoveParentNode, DropItemSprite* pDropItemSprite)
{
    // mapManagerから消す
    MapManager::getInstance()->removeMapItem(pDropItemSprite->getDropMapItem());
    
    // ミニマップを更新
    getGridSpriteBatchNode()->removeChildByTag(pDropItemSprite->getTag());
    
    // Map上からremoveする
    pRemoveParentNode->removeChild(pDropItemSprite);
}

SpriteBatchNode* RogueScene::getGridSpriteBatchNode()
{
    auto pMiniMapLayer = getChildByTag(RogueScene::MiniMapLayerTag);
    if (pMiniMapLayer)
    {
        auto pBatchNode = static_cast<SpriteBatchNode*>(pMiniMapLayer->getChildByTag(MiniMapLayerTags::MiniMapSimbolBatchNodeTag));
        if (!pBatchNode)
        {
            pBatchNode = SpriteBatchNode::create("grid32.png");
            pBatchNode->setTag(MiniMapLayerTags::MiniMapSimbolBatchNodeTag);
            pMiniMapLayer->addChild(pBatchNode);
        }
        return pBatchNode;
    }
    return nullptr;
}

void RogueScene::addMiniMapItem(MapItem* pMapItem, int baseSpriteTag)
{
    int index = 0;
    Color3B spriteColor;
    int alpha = 255;
    if (pMapItem->mapDataType == MapDataType::MAP_ITEM)
    {
        spriteColor = Color3B(51, 204, 255);
        index = RogueScene::MiniMapLayerMapItemZOrder;
    }
    else if (pMapItem->mapDataType == MapDataType::ENEMY)
    {
        spriteColor = Color3B::RED;
        index = RogueScene::MiniMapLayerMapActorZOrder;
    }
    else if (pMapItem->mapDataType == MapDataType::PLAYER)
    {
        spriteColor = Color3B::YELLOW;
        index = RogueScene::MiniMapLayerMapActorZOrder;
    }
    else if (pMapItem->mapDataType == MapDataType::NONE)
    {
        spriteColor = Color3B(0, 0, 196);
        alpha = 128;
        index = RogueScene::MiniMapLayerMapNoneZOrder;
    }
    else if (pMapItem->mapDataType == MapDataType::KAIDAN)
    {
        spriteColor = Color3B::GREEN;
        alpha = 128;
        index = RogueScene::MiniMapLayerMapItemZOrder;
    }
    
    auto pBatchNode = getGridSpriteBatchNode();
    if (pBatchNode)
    {
        auto pSprite = Sprite::createWithTexture(pBatchNode->getTexture());
        pSprite->setPosition(indexToPoint(pMapItem->mapIndex.x, pMapItem->mapIndex.y));
        
        pSprite->setColor(spriteColor);
        pSprite->setOpacity(alpha);
        
        // タイルの1/8サイズ
        float scale = 1.0f / MINI_MAP_SCALE;
        pSprite->setScale(scale);
        pSprite->setContentSize(base_tile_size_ / MINI_MAP_SCALE);
        // 現在位置からPositionを取得して1/8にする
        auto point = indexToPointNotTileSize(pMapItem->mapIndex) / MINI_MAP_SCALE;
        pSprite->setPosition(point.x + pSprite->getContentSize().width / 2 * scale,
                             point.y + pSprite->getContentSize().height / 2 * scale);
        // 移動時に更新できるようにtag管理
        pSprite->setTag(baseSpriteTag);
        pSprite->setZOrder(index);
        
        // add
        pBatchNode->addChild(pSprite);
    }
}

void RogueScene::refreshStatus()
{
    auto pStatusBarLayer = getChildByTag(RogueScene::StatusBarLayerTag);
    if (!pStatusBarLayer)
    {
        return;
    }
    
    auto pStatusText = pStatusBarLayer->getChildren().at(0); // TODO: とりあえず1要素なので。。。
    if (pStatusText)
    {
        // プレイヤー取得
        auto pPlayerSprite = getPlayerActorSprite(1);
        auto pPlayerDto = pPlayerSprite->getActorDto();
        int floor = rogue_play_data_.quest_id; // フロア情報（クエストID=フロア数でいい？)
        int gold = 0; // TODO: player情報
        // 作成
        auto pStr = String::createWithFormat(" %2dF Lv%3d HP %3d/%3d 満腹度 %d/%d %10d G", floor, pPlayerDto->lv, pPlayerDto->hitPoint, pPlayerDto->hitPointLimit, pPlayerDto->magicPoint, pPlayerDto->magicPointLimit, gold);
        
        auto pLabelText = static_cast<LabelTTF*>(pStatusText);
        pLabelText->setString(pStr->getCString());
        pLabelText->setPositionX(pLabelText->getContentSize().width / 2);
        
        // TODO: 死亡判定ここで？
        if (pPlayerDto->hitPoint == 0)
        {
            logMessage("%sは死亡した。", pPlayerDto->name.c_str());
            changeGameStatus(RogueScene::GameStatus::GAME_OVER);
        }
        else if (pPlayerDto->magicPoint == 0)
        {
            logMessage("%sは空腹で倒れた。", pPlayerDto->name.c_str());
            changeGameStatus(RogueScene::GameStatus::GAME_OVER);
        }
        else
        {
            // 残りHPで文字色を変える
            float hitPointDiv = (float)pPlayerDto->hitPoint / (float)pPlayerDto->hitPointLimit;
            float mpDiv = (float)pPlayerDto->magicPoint / (float)pPlayerDto->magicPointLimit;
            if (hitPointDiv <= 0.25 || mpDiv <= 0.10)
            {
                pLabelText->setColor(Color3B::RED);
            }
            else if (hitPointDiv <= 0.50 || mpDiv <= 0.30)
            {
                pLabelText->setColor(Color3B::YELLOW);
            }
            else
            {
                pLabelText->setColor(Color3B::WHITE);
            }
        }
    }
}

#pragma mark
#pragma mark 照明
void RogueScene::tiledMapLighting()
{
    // プレイヤー周辺３＊３はかならず更新
    Rect floorInfoPlayerIndexRect = createPlayerRect(1);
    refreshAutoMapping(floorInfoPlayerIndexRect);
    // プレイヤー視野でマップをリフレッシュする
    tiledMapItemLighting(floorInfoPlayerIndexRect, true);
    
    Rect floorInfoIndexRect = getTileMapFloorInfo();
    if (floorInfoIndexRect.equals(Rect::ZERO))
    {
        // 視野をプレイヤー周辺に更新
        hideFloorLighting();
        showPlayerLighting();
    }
    else
    {
        // 視野を部屋に更新
        hidePlayerLighting();
        showFloorLighting(floorInfoIndexRect);
        // マッピング更新
        refreshAutoMapping(floorInfoIndexRect);
        
        // マップ情報も更新
        tiledMapItemLighting(floorInfoIndexRect, false);
    }
}

/**
 * タイルマップ上のマップアイテムを視野確認を更新。（ミニマップも）
 *
 * @param floorInfoIndexRect 表示する領域
 * @param isRefresh true : 表示をリフレッシュ / false : 表示中を非表示にはしない
 */
void RogueScene::tiledMapItemLighting(const Rect& floorInfoIndexRect, bool isRefresh)
{
    auto pBatchNode = getGridSpriteBatchNode();
    
    // 今いる部屋以外見えなくする
    auto pTiledMapLayer = getChildByTag(RogueScene::TiledMapLayerTag);
    
    // アイテム
    auto pDropItemMapLayer = pTiledMapLayer->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
    if (pDropItemMapLayer)
    {
        for (auto pDropItemNode : pDropItemMapLayer->getChildren())
        {
            bool isContains = floorInfoIndexRect.containsPoint(pDropItemNode->getPosition());
            if (isRefresh || isContains)
            {
                pDropItemNode->setVisible(isContains);
                
                if (pBatchNode)
                {
                    auto pMiniDropItemNode = pBatchNode->getChildByTag(pDropItemNode->getTag());
                    if (pMiniDropItemNode)
                    {
                        pMiniDropItemNode->setVisible(isContains);
                    }
                }
            }
        }
    }
    
    //階段
    auto pKaidan = pTiledMapLayer->getChildByTag(TiledMapTags::TiledMapObjectTag);
    if (pKaidan)
    {
        if (!pKaidan->isVisible()) {
            bool isContains = floorInfoIndexRect.containsPoint(pKaidan->getPosition());
            if (isRefresh || isContains)
            {
                pKaidan->setVisible(isContains);
                
                if (pBatchNode)
                {
                    auto pMiniKaidanNode = pBatchNode->getChildByTag(pKaidan->getTag());
                    if (pMiniKaidanNode)
                    {
                        pMiniKaidanNode->setVisible(isContains);
                    }
                }
            }
        }
    }
    
    // 敵
    auto pEnemyMapLayer = pTiledMapLayer->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    if (pEnemyMapLayer)
    {
        for (auto pEnemyNode : pEnemyMapLayer->getChildren())
        {
            bool isContains = floorInfoIndexRect.containsPoint(pEnemyNode->getPosition());
            if (isRefresh || isContains)
            {
                pEnemyNode->setVisible(isContains);
                
                if (pBatchNode)
                {
                    auto pMiniEnemyNode = pBatchNode->getChildByTag(pEnemyNode->getTag());
                    if (pMiniEnemyNode)
                    {
                        pMiniEnemyNode->setVisible(isContains);
                    }
                }
            }
        }
    }
}

Rect RogueScene::getTileMapFloorInfo()
{
    auto pActorSprite = getPlayerActorSprite(1);
    auto pTileMapLayer = getChildByTag(RogueScene::TiledMapLayerTag);
    
    for (auto& child : pTileMapLayer->getChildren())
    {
        TMXLayer* layer = dynamic_cast<TMXLayer*>(child);
        if(layer)
        {
            if(layer->getLayerName().compare("background") == 0 || layer->getLayerName().compare("colision") == 0)
            {
                continue;
            }
            
            MapIndex tiledIndex = mapIndexToTileIndex(pActorSprite->getActorMapItem()->mapIndex);
            auto pTile = layer->getTileAt(Point(tiledIndex.x, tiledIndex.y));
            if (pTile)
            {
                MapIndex tileMapIndex = mapIndexToTileIndex(layer->getProperty("x").asInt(), layer->getProperty("y").asInt());
                // TODO: メタデータでyが上から設定されているので逆転している あとindexなので1足してる
                tileMapIndex.y = tileMapIndex.y - layer->getProperty("height").asInt() + 1;
                Point floorMaskPoint = indexToPoint(tileMapIndex);
                Size floorMaskSize = Size(layer->getProperty("width").asInt() * base_tile_size_.width, layer->getProperty("height").asInt() * base_tile_size_.height);
                CCLOG("x[%f] y[%f] w[%f] h[%f]", floorMaskPoint.x, floorMaskPoint.y, floorMaskSize.width, floorMaskSize.height);
                return Rect(floorMaskPoint.x - base_tile_size_.width / 2, floorMaskPoint.y - base_tile_size_.height / 2, floorMaskSize.width, floorMaskSize.height);
            }
        }
    }
    return Rect::ZERO;
}

void RogueScene::showPlayerLighting() {
    auto winSize = Director::getInstance()->getWinSize();
    
    auto base_layer = dynamic_cast<LayerColor*>(this->getChildByTag(RogueScene::RoguePlayerLightBaseTag));
    if (base_layer) {
        base_layer->setVisible(true);
    } else {
        base_layer = LayerColor::create();
        this->addChild(base_layer, RogueScene::RoguePlayerLightBaseZOrder, RogueScene::RoguePlayerLightBaseTag);
    }

    // フロントレイヤーにマップを暗くするやつを設定
    auto mask_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(RogueScene::RoguePlayerLightMaskTag));
    if (mask_layer) {
        mask_layer->setVisible(true);
    } else {
        mask_layer = LayerColor::create(Color4B(0,0,0,128));
        mask_layer->setContentSize(winSize);
        mask_layer->setPosition(Point::ZERO);
        base_layer->addChild(mask_layer, RogueScene::RoguePlayerLightMaskZOrder, RogueScene::RoguePlayerLightMaskTag);
    }

    auto light_draw = dynamic_cast<DrawNode*>(mask_layer->getChildByTag(RogueScene::RoguePlayerLightTag));
    if (light_draw) {
        light_draw->setVisible(true);
    } else {
        auto actor_sprite = getPlayerActorSprite(1);
        
        light_draw = DrawNode::create();
        light_draw->drawDot(actor_sprite->getPosition(), base_tile_size_.width * 3.0f / 2.0f, Color4F::WHITE);
        mask_layer->addChild(light_draw, RogueScene::RoguePlayerLightZOrder, RogueScene::RoguePlayerLightTag);
        
        BlendFunc blend;
        blend.src = GL_DST_COLOR;
        blend.dst = GL_ONE;
        light_draw->setBlendFunc(blend);
    }
}

void RogueScene::hidePlayerLighting() {
    auto rogue_player_light_base_layer = this->getChildByTag(RogueScene::RoguePlayerLightBaseTag);
    if (rogue_player_light_base_layer) {
        rogue_player_light_base_layer->setVisible(false);
    }
}

void RogueScene::showFloorLighting(const Rect floorInfoIndexRect) {
    auto tile_map_layer = getChildByTag(RogueScene::TiledMapLayerTag);
    auto base_layer = tile_map_layer->getChildByTag(TiledMapTags::TiledMapFrontLayerTag);
    if (base_layer) {
        base_layer->setVisible(true);
        
        // フロントレイヤーにマップを暗くするやつを設定
        auto mask_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(TiledMapTags::FloorLayerTag));
        if (!mask_layer) {
            mask_layer = LayerColor::create(Color4B(0,0,0,128));
            mask_layer->setContentSize(tile_map_layer->getContentSize());
            mask_layer->setPosition(Point::ZERO);
            
            base_layer->addChild(mask_layer, TiledMapIndexs::FloorLayerZOrder, TiledMapTags::FloorLayerTag);
        }

        // 部屋の明かり
        auto floor_light_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(TiledMapTags::FloorMaskLayerTag));
        if (!floor_light_layer) {
            floor_light_layer = LayerColor::create(Color4B(255,255,255,0));
            base_layer->addChild(floor_light_layer, TiledMapIndexs::FloorMaskLayerZOrder, TiledMapTags::FloorMaskLayerTag);
        }
        floor_light_layer->setContentSize(floorInfoIndexRect.size);
        floor_light_layer->setPosition(Point(floorInfoIndexRect.getMinX(), floorInfoIndexRect.getMinY()));
        
        BlendFunc blendFloor;
        blendFloor.src = GL_DST_COLOR;
        blendFloor.dst = GL_ONE;
        
        floor_light_layer->setBlendFunc(blendFloor);

        // プレイヤー周辺の明かり
        auto player_light_layer = dynamic_cast<LayerColor*>(floor_light_layer->getChildByTag(TiledMapTags::FloorMaskPlayerLayerTag));
        if (!player_light_layer) {
            player_light_layer = LayerColor::create(Color4B(255,255,255,0));
            floor_light_layer->addChild(player_light_layer, TiledMapIndexs::FloorMaskLayerZOrder, TiledMapTags::FloorMaskPlayerLayerTag);
        }
        
        // プレイヤー前方のみ
        Rect playerRect = createPlayerRect(1); // プレイヤー周辺3*3
        if (playerRect.getMinX() < floorInfoIndexRect.getMinX()) {
            // 左
            player_light_layer->setContentSize(Size(playerRect.size.width / 3, playerRect.size.height));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMinY() < floorInfoIndexRect.getMinY()) {
            // 下
            player_light_layer->setContentSize(Size(playerRect.size.width, playerRect.size.height / 3));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMaxX() > floorInfoIndexRect.getMaxX()) {
            // 右
            player_light_layer->setContentSize(Size(playerRect.size.width / 3, playerRect.size.height));
            player_light_layer->setPosition(Point(playerRect.getMaxX() - player_light_layer->getContentSize().width, playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMaxY() > floorInfoIndexRect.getMaxY()) {
            // 上
            player_light_layer->setContentSize(Size(playerRect.size.width, playerRect.size.height / 3));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMaxY() - player_light_layer->getContentSize().height) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else {
            player_light_layer->setVisible(false);
        }
    }
}

void RogueScene::hideFloorLighting() {
    auto pTileMapLayer = getChildByTag(RogueScene::TiledMapLayerTag);
    if (pTileMapLayer) {
        auto pFrontLayer = pTileMapLayer->getChildByTag(TiledMapTags::TiledMapFrontLayerTag);
        if (pFrontLayer) {
            pFrontLayer->setVisible(false);
        }
    }
}

Rect RogueScene::createPlayerRect(int rectSize)
{
    auto pActorSprite = getPlayerActorSprite(1);
    if (pActorSprite) {
        MapIndex actorRectMinMapIndex = {
            pActorSprite->getActorMapItem()->mapIndex.x - rectSize,
            pActorSprite->getActorMapItem()->mapIndex.y - rectSize,
            MoveDirectionType::MOVE_NONE
        };
        Point actorRectMinMapPoint = indexToPointNotTileSize(actorRectMinMapIndex);
        // プレイヤー周辺のrectを作成
        return Rect(actorRectMinMapPoint.x, actorRectMinMapPoint.y,
                    base_tile_size_.width * (rectSize * 2 + 1),
                    base_tile_size_.width * (rectSize * 2 + 1));
    } else {
        return Rect::ZERO;
    }
}

void RogueScene::refreshAutoMapping(const Rect& floorInfoIndexRect)
{
    MapIndex minMapIndex = pointToIndex(Point(floorInfoIndexRect.getMinX() + base_tile_size_.width / 2, floorInfoIndexRect.getMinY() + base_tile_size_.height / 2));
    MapIndex maxMapIndex = pointToIndex(Point(floorInfoIndexRect.getMaxX() + base_tile_size_.width / 2, floorInfoIndexRect.getMaxY()+ base_tile_size_.height / 2));
    
    auto pBatchNode = getGridSpriteBatchNode();
    auto mappingData = MapManager::getInstance()->getMappingData();
    for (int x = minMapIndex.x; x < maxMapIndex.x; x++)
    {
        for (int y = minMapIndex.y; y < maxMapIndex.y; y++)
        {
            MapIndex mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
            auto tileMapIndex = mapIndexToTileIndex(mapIndex);
            // TODO: タグが強引すぎる・・・
            int tag = 10000 * tileMapIndex.x + 100 * tileMapIndex.y;
            auto pNode = pBatchNode->getChildByTag(tag);
            if (pNode)
            {
                MapManager::getInstance()->addMapping(tileMapIndex);
                pNode->setVisible(true);
            }
        }
    }
}

#pragma mark
#pragma mark マップ座標変換

Point RogueScene::indexToPoint(MapIndex mapIndex)
{
    return indexToPoint(mapIndex.x, mapIndex.y);
}

Point RogueScene::indexToPoint(int mapIndex_x, int mapIndex_y)
{
    // タイルサイズを考慮
    return Point(base_tile_size_.width * mapIndex_x + base_tile_size_.width * 0.5, base_tile_size_.height * mapIndex_y + base_tile_size_.height * 0.5);
}

MapIndex RogueScene::pointToIndex(Point point)
{
    // タイルサイズを考慮
    point.x = point.x - base_tile_size_.width * 0.5;
    point.y = point.y - base_tile_size_.height * 0.5;
    return touchPointToIndex(point);
}

MapIndex RogueScene::touchPointToIndex(Point point)
{
    MapIndex mapIndex;
    mapIndex.x = point.x / base_tile_size_.width;
    mapIndex.y = point.y / base_tile_size_.height;
    return mapIndex;
}

Point RogueScene::indexToPointNotTileSize(MapIndex mapIndex)
{
    return indexToPointNotTileSize(mapIndex.x, mapIndex.y);
}

Point RogueScene::indexToPointNotTileSize(int mapIndex_x, int mapIndex_y)
{
    return Point(base_tile_size_.width * mapIndex_x, base_tile_size_.height * mapIndex_y);
}

MapIndex RogueScene::mapIndexToTileIndex(MapIndex mapIndex)
{
    return mapIndexToTileIndex(mapIndex.x, mapIndex.y);
}
MapIndex RogueScene::mapIndexToTileIndex(int mapIndex_x, int mapIndex_y)
{
    MapIndex tileIndex;
    tileIndex.x = mapIndex_x;
    tileIndex.y = base_map_size_.height - mapIndex_y - 1;
    return tileIndex;
}

#pragma mark
#pragma mark 汎用

ActorSprite* RogueScene::getPlayerActorSprite(int seqNo)
{
    return static_cast<ActorSprite*>(getChildByTag(RogueScene::ActorPlayerTag + seqNo));
}

ActorSprite* RogueScene::getEnemyActorSprite(int seqNo)
{
    auto pEnemyMapLayer = getChildByTag(RogueScene::TiledMapLayerTag)->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    return static_cast<ActorSprite*>(pEnemyMapLayer->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag + seqNo));
}

ItemWindowLayer* RogueScene::getItemWindowLayer()
{
    auto pModalLayer = getModalLayer();
    return static_cast<ItemWindowLayer*>(pModalLayer->getChildByTag(RogueScene::ItemListWindowTag));
}

ModalLayer* RogueScene::getModalLayer()
{
    // モーダルレイヤー作成
    auto pModalLayer = static_cast<ModalLayer*>(this->getChildByTag(RogueScene::ModalLayerTag));
    if (pModalLayer)
    {
        //pModalLayer->setVisible(true);
    }
    else
    {
        pModalLayer = ModalLayer::create();
        this->addChild(pModalLayer, RogueScene::ModalLayerZOrder, RogueScene::ModalLayerTag);
    }
    
    return pModalLayer;
}

#pragma mark
#pragma mark その他

int GetRandom(int min,int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

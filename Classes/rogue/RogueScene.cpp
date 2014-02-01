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
:m_gameStatus(GameStatus::INIT),
m_questId(0),
m_TurnCount(0),
m_enemyCount(0),
m_baseMapSize(Size::ZERO),
m_baseTileSize(Size::ZERO),
m_baseContentSize(Size::ZERO)
, m_listener(nullptr)
{
    CCLOG("new rogueScene");
    //this->getEventDispatcher()->removeAllEventListeners();
}

RogueScene::~RogueScene()
{
    CCLOG("death rogueScene");
    this->getEventDispatcher()->removeEventListener(m_listener);
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
    if ( !Layer::init() )
    {
        return false;
    }
    
    m_questId = questId;
    
    // 乱数
    srand((unsigned int)time(NULL));
    
    // TouchEvent settings
    auto listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(RogueScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(RogueScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(RogueScene::onTouchEnded, this);

    // こういう書き方もできる
//    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
//        return true;
//    };
//    listener->onTouchMoved = [this](Touch* touch, Event* event) {
//        
//    };
//    listener->onTouchEnded = [this](Touch* touch, Event* event) {
//        
//    };
    
    this->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);
    m_listener = listener;
    
    auto winSize = Director::getInstance()->getWinSize();
    
    // ---------------------
    // タイルマップを生成
    // ---------------------
//    auto pTiledMap = TMXTiledMap::create("tmx/desert.tmx");
#if 1
    // TODO: テスト用 まだクエスト2までしかない・・・
    if (m_questId > 2)
    {
        m_questId = 1;
    }
#endif
//    auto pTiledMap = TMXTiledMap::create(StringUtils::format("tmx/quest_%d.tmx", m_questId));
    auto pTiledMap = TMXTiledMap::create(StringUtils::format("tmx/quest_%d_2.tmx", 2));

    pTiledMap->setPosition(Point::ZERO);
    this->addChild(pTiledMap, RogueScene::TiledMapLayerZOrder, RogueScene::kTiledMapTag);
    
    m_baseMapSize = pTiledMap->getMapSize();
    m_baseTileSize = pTiledMap->getTileSize();
    m_baseContentSize = pTiledMap->getContentSize();
    
    m_mapManager.init(0, (int)m_baseMapSize.height, 0, (int)m_baseMapSize.width);

    // 使ってなかった
//    // フロントレイヤー
//    auto pFrontLayer = Layer::create();
//    pTiledMap->addChild(pFrontLayer,
//                        RogueScene::TiledMapIndex::zTiledMapFrontIndex,
//                        RogueScene::TiledMapTag::kTiledMapFrontTag);
    
    // エネミーレイヤー
    auto pEnemyLayer = Layer::create();
    pTiledMap->addChild(pEnemyLayer,
                        RogueScene::TiledMapIndex::zTiledMapEnemyBaseIndex,
                        RogueScene::TiledMapTag::kTiledMapEnemyBaseTag);
    
    // ドロップアイテムレイヤー
    auto pDropItemLayer = Layer::create();
    pTiledMap->addChild(pDropItemLayer,
                        RogueScene::TiledMapIndex::zTiledMapDropItemBaseIndex,
                        RogueScene::TiledMapTag::kTiledMapDropItemBaseTag);
    
    // ---------------------
    // グリッド線を生成
    // ---------------------
#if 0
    auto pGrid = createGridDraw();
    // マップに追加
    pTiledMap->addChild(pGrid, RogueScene::TiledMapIndex::zGridLineIndex, RogueScene::TiledMapTag::kGridLineTag);
#endif
    
    //-------------------------
    // ステータスバー？
    //-------------------------
    auto statusLayer = LayerColor::create(Color4B::BLACK);
    statusLayer->setContentSize(Size(winSize.width, m_baseTileSize.height * 0.8));
    statusLayer->setPosition(Point(0, winSize.height - statusLayer->getContentSize().height));
    
    // 更新する
    auto sampleText = LabelTTF::create(" --F Lv-- HP ---/--- 満腹度 ---/---          - G", GAME_FONT(16), 16);
    
    sampleText->setPosition(Point(sampleText->getContentSize().width / 2, statusLayer->getContentSize().height / 2));
    statusLayer->addChild(sampleText);
    
    this->addChild(statusLayer, RogueScene::StatusBarLayerZOrder, RogueScene::kStatusBarTag);
    
    //-------------------------
    // ゲームログ表示
    //-------------------------
    auto pGameLogLayer = LayerColor::create(Color4B(0, 0, 0, 192));
    pGameLogLayer->setContentSize(Size(winSize.width * 0.5, m_baseTileSize.height * 1.5));
    pGameLogLayer->setPosition(winSize.width / 2 - pGameLogLayer->getContentSize().width / 2, 0);
    
    CommonWindowUtil::attachWindowWaku(pGameLogLayer);
    
    int baseFontSize = 10;
    auto pLogTextLabel = LabelTTF::create("", GAME_FONT(baseFontSize), baseFontSize, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
    pLogTextLabel->setPosition(Point(pLogTextLabel->getContentSize().width / 2 + pLogTextLabel->getFontSize() / 4, pGameLogLayer->getContentSize().height - pLogTextLabel->getContentSize().height / 2 - pLogTextLabel->getFontSize() / 4));
    pGameLogLayer->addChild(pLogTextLabel);
    
    this->addChild(pGameLogLayer, RogueScene::GameLogLayerZOrder, RogueScene::kGameLogTag);
    
    // ------------------------
    // ミニマップ
    // ------------------------
    // 透明
    auto miniMapLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    // 1/8サイズ
    miniMapLayer->setContentSize(Size(m_baseMapSize.width * m_baseTileSize.width / 8,
                                      m_baseMapSize.height * m_baseTileSize.height / 8));
    // ステータスバーの下くらい
    miniMapLayer->setPosition(0, miniMapLayer->getPositionY() + winSize.height - miniMapLayer->getContentSize().height - statusLayer->getContentSize().height);
    this->addChild(miniMapLayer, RogueScene::MiniMapLayerZOrder, RogueScene::MiniMapLayerTag);
    
    // ------------------------
    // 障害物
    // ------------------------
    
    // 障害物をmapManagerに適応する
    auto pColisionLayer = pTiledMap->getLayer("colision");
    for (int x = 0; x < m_baseMapSize.width; x++)
    {
        for (int y = 0; y < m_baseMapSize.height; y++)
        {
            MapIndex mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
            auto tileMapIndex = mapIndexToTileIndex(mapIndex);
            if (pColisionLayer->getTileAt(Point(x, y)))
            {
                m_mapManager.addObstacle(&tileMapIndex);
            }
            else
            {
                // ミニマップ更新
                addMiniMapItem(m_mapManager.getMapItem(&tileMapIndex), -1);
            }
        }
    }
    
    // ------------------------
    // イベントリ作成
    // ------------------------
    showItemList(1);
    hideItemList();
    
    // ---------------------
    // プレイヤー生成
    // ---------------------
    ActorSprite::ActorDto actorDto;
    actorDto.name = "ジニー";
    actorDto.faceImgId = 0;
    actorDto.imageResId = 1015;
    // 基本
    actorDto.attackRange = 1;
    actorDto.movePoint = 5;
    actorDto.playerId = 4;
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
    
    ActorMapItem actorMapItem;
    actorMapItem.mapDataType = MapDataType::PLAYER;
    // 画面の中心（固定）
    actorMapItem.mapIndex = pointToIndex(Point(winSize.width/2, winSize.height/2));
    actorMapItem.seqNo = 1;
    actorMapItem.moveDist = actorDto.movePoint;
    actorMapItem.attackDist = actorDto.attackRange;
    actorMapItem.moveDone = false;
    actorMapItem.attackDone = false;
    
    auto actorSprite = ActorSprite::createWithActorDto(actorDto);
    actorSprite->setPosition(indexToPoint(actorMapItem.mapIndex));
    actorSprite->setActorMapItem(actorMapItem);
    actorSprite->runBottomAction();
    // プレイヤーは画面中心にくるのでmapLayerに追加しない
    this->addChild(actorSprite, RogueScene::ActorPlayerZOrder, (RogueScene::ActorPlayerTag + actorMapItem.seqNo));
    
    // マップに追加
    m_mapManager.addActor(actorSprite->getActorMapItem());
    
    refreshStatus();
    
    // プレイヤーの位置表示用（同じく1/8サイズ）
    addMiniMapItem(actorSprite->getActorMapItem(), actorSprite->getTag());
    
    // プレイヤー位置の移動
    MapIndex playerRandMapIndex = getRandomMapIndex(false, true);
    MapIndex moveIndex = {
        playerRandMapIndex.x - actorMapItem.mapIndex.x,
        playerRandMapIndex.y - actorMapItem.mapIndex.y,
        actorMapItem.mapIndex.moveDictType
    };
    moveMap(moveIndex, actorMapItem.seqNo, MapDataType::PLAYER, NULL);
    
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
    DropItemSprite::DropItemDto dropItemDto;
    dropItemDto.objectId = 1; // 単純に連番でいい
    dropItemDto.itemId = 1;
    dropItemDto.itemType = DropItemSprite::ItemType::HP_RECOVER_VALUE;
    dropItemDto.imageResId = 641;
    dropItemDto.name = "ポーション";
    dropItemDto.isEquip = false;
    
    MapIndex mapIndex = getRandomMapIndex(false, false);
    tileSetDropMapItem(dropItemDto, mapIndex);

    DropItemSprite::DropItemDto dropItemDto2;
    dropItemDto2.objectId = 2;
    dropItemDto2.itemId = 2;
    dropItemDto2.itemType = DropItemSprite::ItemType::MP_RECOVER_VALUE;
    dropItemDto2.imageResId = 645;
    dropItemDto2.name = "エーテル";
    dropItemDto2.isEquip = false;
    
    MapIndex mapIndex2 = getRandomMapIndex(false, false);;
    tileSetDropMapItem(dropItemDto2, mapIndex2);

    DropItemSprite::DropItemDto dropItemDto3;
    dropItemDto3.objectId = 3;
    dropItemDto3.itemId = 3; // weaponId
    dropItemDto3.itemType = DropItemSprite::ItemType::EQUIP_WEAPON;
    dropItemDto3.imageResId = 816;
    dropItemDto3.name = "木の剣";
    dropItemDto3.isEquip = false;
    
    MapIndex mapIndex3 = getRandomMapIndex(false, false);;
    tileSetDropMapItem(dropItemDto3, mapIndex3);

    DropItemSprite::DropItemDto dropItemDto4;
    dropItemDto4.objectId = 4;
    dropItemDto4.itemId = 4; // accessoryId
    dropItemDto4.itemType = DropItemSprite::ItemType::EQUIP_ACCESSORY;
    dropItemDto4.imageResId = 1040;
    dropItemDto4.name = "木の盾";
    dropItemDto4.isEquip = false;
    
    MapIndex mapIndex4 = getRandomMapIndex(false, false);;
    tileSetDropMapItem(dropItemDto4, mapIndex4);

    // -------------------------------
    // 階段配置
    // -------------------------------
    
    MapIndex kaidanIndex = getRandomMapIndex(false, false);
    auto pKaidanSprite = Sprite::create("kaidan.png");
    pKaidanSprite->setPosition(indexToPoint(kaidanIndex));
    pTiledMap->addChild(pKaidanSprite, RogueScene::zTiledMapObjectIndex, RogueScene::kTiledMapObjectTag);
    
    // マップに追加
    DropMapItem kaidanMapItem;
    kaidanMapItem.seqNo = 1;
    kaidanMapItem.mapIndex = kaidanIndex;
    kaidanMapItem.moveDist = 0;
    kaidanMapItem.attackDist = 0;
    kaidanMapItem.mapDataType = MapDataType::KAIDAN;
    m_mapManager.addDropItem(&kaidanMapItem);
    
    // ミニマップも更新
    addMiniMapItem(&kaidanMapItem, RogueScene::kTiledMapObjectTag);
    
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
    // プレイヤーの周り以外暗くする
    auto mask = DrawNode::create();
    mask->drawDot(actorSprite->getPosition(), m_baseTileSize.width * 3.0f / 2.0f, Color4F::WHITE);
//    auto mask = LayerColor::create(Color4B(255,255,255,255));
//    mask->setContentSize(m_baseTileSize * 3);
//    mask->setPosition(Point(actorSprite->getPosition().x - mask->getContentSize().width / 2, actorSprite->getPosition().y - mask->getContentSize().height / 2));

    
    auto pRogueLayer = LayerColor::create(Color4B(0,0,0,128));
    pRogueLayer->setContentSize(winSize);
    pRogueLayer->setPosition(Point(winSize.width / 2 - pRogueLayer->getContentSize().width / 2, winSize.height / 2 - pRogueLayer->getContentSize().height / 2));

    
    this->addChild(pRogueLayer, RogueScene::RogueZOrder, RogueScene::RogueLayerTag);
    this->addChild(mask, RogueScene::RogueMaskZOrder, RogueScene::RogueMaskLayerTag);
    
    BlendFunc blend;
    blend.src = GL_DST_COLOR;
    blend.dst = GL_ONE;
    
    mask->setBlendFunc(blend);
    
    
//    for (auto& child : pTiledMap->getChildren())
//    {
//        TMXLayer* layer = dynamic_cast<TMXLayer*>(child);
//        if(layer)
//        {
//            if(layer->getLayerName().compare("background") == 0 || layer->getLayerName().compare("colision") == 0)
//            {
//                continue;
//            }
//            auto pTile = layer->getTileAt(indexToPoint(actorSprite->getActorMapItem()->mapIndex));
//            if (pTile)
//            {
//                // このフロアを明るくする
//                layer->getProperty("x");
//                layer->getProperty("y");
//                layer->getProperty("width");
//                layer->getProperty("height");
//                break;
//            }
//        }
//    }
//    
//    // フロアの明かり
//    auto floorMask = LayerColor::create(Color4B(255,255,255,0));
//    floorMask->setContentSize(m_baseTileSize * 3);
//    Point floorMaskPoint = indexToPoint(actorSprite->getActorMapItem()->mapIndex);
//    
//    floorMask->setPosition(Point(floorMaskPoint.x - floorMask->getContentSize().width / 2, floorMaskPoint.y - floorMask->getContentSize().height / 2));
//    
//    auto pFloorRogueLayer = LayerColor::create(Color4B(0,0,0,128));
//    pFloorRogueLayer->setContentSize(pTiledMap->getContentSize());
//    pFloorRogueLayer->setPosition(Point(pTiledMap->getContentSize().width / 2 - pFloorRogueLayer->getContentSize().width / 2, pTiledMap->getContentSize().height / 2 - pFloorRogueLayer->getContentSize().height / 2));
//    
//    pTiledMap->addChild(pFloorRogueLayer, RogueScene::FloorLayerZOrder, RogueScene::FloorLayerTag);
//    pTiledMap->addChild(floorMask, RogueScene::FloorMaskLayerZOrder, RogueScene::FloorMaskLayerTag);
//    
//    BlendFunc blendFloor;
//    blendFloor.src = GL_DST_COLOR;
//    blendFloor.dst = GL_ONE;
//    floorMask->setBlendFunc(blendFloor);
    
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
    for (int x = 1; x < m_baseMapSize.width; x++)
    {
        float xPoint = x * m_baseTileSize.width;
        draw->drawSegment(Point(xPoint, 0), Point(xPoint, m_baseContentSize.height), lineSize, color);
    }
    // 横線を引く
    for (int y = 1; y < m_baseMapSize.height; y++)
    {
        float yPoint = y * m_baseTileSize.height;
        draw->drawSegment(Point(0, yPoint), Point(m_baseContentSize.width, yPoint), lineSize, color);
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
    
    auto pMenuKeyUp = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Object *pSender) {
        CCLOG("pMenuKeyUpが押された！");
        if (m_gameStatus == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = this->pointToIndex(point);
            this->touchEventExec(this->indexToPoint(mapIndex.x, mapIndex.y + 1));
        }
    });
    pMenuKeyUp->setPosition(indexToPoint(1, 2));
    resultArray.pushBack(pMenuKeyUp);
    
    auto pMenuKeyRight = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Object *pSender) {
        CCLOG("pMenuKeyRightが押された！");
        if (m_gameStatus == GameStatus::PLAYER_TURN)
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
    
    auto pMenuKeyDown = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Object *pSender) {
        CCLOG("pMenuKeyDownが押された！");
        if (m_gameStatus == GameStatus::PLAYER_TURN)
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
    
    auto pMenuKeyLeft = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Object *pSender) {
        CCLOG("pMenuKeyLeftが押された！");
        if (m_gameStatus == GameStatus::PLAYER_TURN)
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
    Vector<MenuItem*> resultArray;
    
    auto a_button = Sprite::create("ui/a_button.png");
    auto a_buttonPress = Sprite::create("ui/a_button_press.png");
    a_buttonPress->setOpacity(128);
    auto pA_MenuButton = MenuItemSprite::create(a_button, a_buttonPress, [this](Object* pSender) {
        CCLOG("Aボタンが押された！");
        if (m_gameStatus == GameStatus::PLAYER_TURN)
        {
            this->attack();
        }
    });
    pA_MenuButton->setPosition(indexToPoint(12, 1));
    pA_MenuButton->setTag(RogueScene::A_ButtonMenuTag);
    resultArray.pushBack(pA_MenuButton);
    
    auto b_button = Sprite::create("ui/b_button.png");
    auto b_buttonPress = Sprite::create("ui/b_button_press.png");
    b_buttonPress->setOpacity(128);
    auto pB_MenuButton = MenuItemSprite::create(b_button, b_buttonPress, [this](Object* pSender) {
        CCLOG("Bボタンが押された！");
    });
    pB_MenuButton->setPosition(indexToPoint(11, 0));
    pB_MenuButton->setTag(RogueScene::B_ButtonMenuTag);
    resultArray.pushBack(pB_MenuButton);
    
    auto c_button = Sprite::create("ui/c_button.png");
    auto c_buttonPress = Sprite::create("ui/c_button_press.png");
    c_buttonPress->setOpacity(128);
    auto pC_MenuButton = MenuItemSprite::create(c_button, c_buttonPress, [this](Object* pSender) {
        CCLOG("Cボタンが押された！");
        
        this->showCommonWindow("階段です。\n　\n次の階に進みますか？", [this](Object* pSender){
            // OK
            this->hideCommonWindow();
            
            // 画面遷移
            this->changeScene(RogueScene::scene(m_questId+1));
            
        }, [this](Object* pSender){
            // NG
            this->hideCommonWindow();
        });
        
    });
    pC_MenuButton->setPosition(indexToPoint(10, 1));
    pC_MenuButton->setTag(RogueScene::C_ButtonMenuTag);
    resultArray.pushBack(pC_MenuButton);
    
    auto d_button = Sprite::create("ui/d_button.png");
    auto d_buttonPress = Sprite::create("ui/d_button_press.png");
    d_buttonPress->setOpacity(128);
    auto pD_MenuButton = MenuItemSprite::create(d_button, d_buttonPress, [this](Object* pSender) {
        CCLOG("Dボタンが押された！");
        showItemList(1);
    });
    pD_MenuButton->setPosition(indexToPoint(11, 2));
    pD_MenuButton->setTag(RogueScene::D_ButtonMenuTag);
    resultArray.pushBack(pD_MenuButton);
    
    return resultArray;
}

#pragma mark
#pragma mark ゲームステート関連

void RogueScene::changeGameStatus(GameStatus gameStatus)
{
    CCLOG("turn %d change gameStatus %d => %d", m_TurnCount, m_gameStatus, gameStatus);
    
    GameStatus beforeGameStatus = m_gameStatus;
    m_gameStatus = gameStatus;
    
    if (m_gameStatus == GameStatus::GAME_OVER)
    {        
        // TODO: ゲームオーバーの演出
        
        // TODO: ゲームオーバー画面Scene？表示
        
        // TODO: とりあえずタイトルへ
        this->changeScene(TitleSceneLoader::scene());
    }
    else if ((beforeGameStatus == GameStatus::PLAYER_TURN || beforeGameStatus == GameStatus::PLAYER_ACTION || beforeGameStatus == GameStatus::PLAYER_NO_ACTION)
        && m_gameStatus == GameStatus::ENEMY_TURN)
    {
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
        
        // 敵のターン開始時
        enemyTurn();
    }
    else if (m_gameStatus == GameStatus::PLAYER_ACTION)
    {
        m_noActionCount = 0;
    }
    else if (m_gameStatus == GameStatus::PLAYER_NO_ACTION)
    {
        m_noActionCount++;
    }
    else if (m_gameStatus == GameStatus::PLAYER_TURN)
    {
        auto pPlayer = getPlayerActorSprite(1);
        
        // カーソルはクリアする
        m_mapManager.clearCursor();
        // ターン数を進める
        m_TurnCount++;
        
        // TODO: とりあえずここで・・・
        auto pPlayerDto = pPlayer->getActorDto();
        
        // 10ターンに1空腹度が減るという
        if (m_TurnCount % 10 == 0)
        {
            if (pPlayerDto->magicPoint > 0)
            {
                pPlayerDto->magicPoint--;
            }
        }
        // 無行動が4ターン続くとHPが回復
        if (m_noActionCount == 4)
        {
            m_noActionCount = 0;
            
            if (pPlayerDto->hitPointLimit > pPlayerDto->hitPoint)
            {
                pPlayerDto->hitPoint++;
            }
        }
        refreshStatus();
    }
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
        randX = GetRandom(0, m_baseMapSize.width-1); // x軸
        randY = GetRandom(0, m_baseMapSize.height-1); // y軸
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
        auto randTargetMapItem = m_mapManager.getMapItem(&randMapIndex);
        
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
    std::list<ActorMapItem> enemyList = m_mapManager.findEnemyMapItem();
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
                m_mapManager.createActorFindDist(enemyMapItem.mapIndex, enemyMapItem.moveDist);
                
                // 最も移動コストがかからない場所を抽出
                MapItem targetMoveDistMapItem = m_mapManager.searchTargetMapItem(searchMapIndexList);
                
                // 移動リスト作成
                if (targetMoveDistMapItem.mapDataType == MapDataType::MOVE_DIST)
                {
                    std::list<MapIndex> moveList = m_mapManager.createMovePointList(targetMoveDistMapItem.mapIndex,
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
                m_mapManager.checkMoveDirectionType(moveMapIndex, pEnemySprite->getActorMapItem()->mapIndex)
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
            else if (m_mapManager.getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::ENEMY)
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
            else if (m_mapManager.getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::PLAYER)
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
    std::list<ActorMapItem> enemyList = m_mapManager.findEnemyMapItem();
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
    if (m_gameStatus == GameStatus::PLAYER_TURN)
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
    if (m_gameStatus == GameStatus::PLAYER_TURN)
    {
        // keypadにしました。
//        auto touchPoint = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));
//
//        // 行動判定
//        touchEventExec(touchPoint);
    }
}

void RogueScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->unscheduleUpdate();
}


void RogueScene::touchEventExec(cocos2d::Point touchPoint)
{
    auto pMapLayer = (TMXTiledMap*)getChildByTag(kTiledMapTag);
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
    auto pEnemyMapItem = m_mapManager.getActorMapItem(&touchPointMapIndex);
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
            auto pTouchPointMapItem = m_mapManager.getMapItem(&touchPointMapIndex);
            if (pTouchPointMapItem->mapDataType == MapDataType::MAP_ITEM)
            {
                // ドロップアイテムを拾う
                auto pDropMapItem = static_cast<DropMapItem*>(pTouchPointMapItem);
                
                // TODO: 拾うSE再生
                
                // itemを取得
                auto pDropItemLayer = getChildByTag(RogueScene::kTiledMapTag)->getChildByTag(RogueScene::TiledMapTag::kTiledMapDropItemBaseTag);
                auto pDropItemSprite = static_cast<DropItemSprite*>(pDropItemLayer->getChildByTag(RogueScene::TiledMapTag::kTiledMapDropItemBaseTag + pDropMapItem->seqNo));
                
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
                // 階段SE
                
                // 階段下りる判定
                
                this->showCommonWindow("階段です。\n　\n次の階に進みますか？", [this](Object* pSender){
                    // OK
                    
                    // 閉じる
                    this->hideCommonWindow();
                    
                    // 画面遷移
                    this->changeScene(RogueScene::scene(m_questId+1));
                    
                }, [this](Object* pSender){
                    // NG
                    
                    // 閉じる
                    this->hideCommonWindow();
                });
            }
            
            // 移動処理
            moveMap(addMoveIndex, pActorSprite->getActorMapItem()->seqNo, pActorSprite->getActorMapItem()->mapDataType, CallFunc::create([this](void) {
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
    auto pEnemyMapItem = m_mapManager.getActorMapItem(&attackMapIndex);
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
                    auto pMLevel = MLevelDao::getInstance()->selectById(pPlayerDto->lv);
                    pPlayerDto->hitPointLimit += pMLevel->getGrowHitPoint();
                    
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
    auto pMapLayer = (TMXTiledMap*)getChildByTag(kTiledMapTag);
    
    // 移動する距離をPointに変換
    auto addMovePoint = Point(m_baseTileSize.width * addMoveIndex.x, m_baseTileSize.height * addMoveIndex.y);
    
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
    m_mapManager.moveActor(actorMapItem, moveMapIndex);
    // actor情報も更新
    actorMapItem->mapIndex = moveMapIndex;
    
    // ミニマップも更新
    auto pMiniMapLayer = getGridSpriteBatchNode();
    if (pMiniMapLayer)
    {
        auto pMiniMapActorNode = pMiniMapLayer->getChildByTag(pActorSprite->getTag());
        
        float scale = 1.0f / 8.0f;
        auto point = indexToPointNotTileSize(pActorSprite->getActorMapItem()->mapIndex) / 8;
        pMiniMapActorNode->setPosition(point.x + pMiniMapActorNode->getContentSize().width / 2 * scale,
                                       point.y + pMiniMapActorNode->getContentSize().height / 2 * scale);
    }
    // move実行
    pActionTargetNode->runAction(Sequence::create(pMoveRunAction, moveFinishedCallFunc, NULL));
}

bool RogueScene::isTiledMapColisionLayer(MapIndex touchPointMapIndex)
{
    // 障害物判定
    auto pMapLayer = static_cast<TMXTiledMap*>(getChildByTag(kTiledMapTag));
    
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
    if (m_baseMapSize.width <= touchPointMapIndex.x ||
        m_baseMapSize.height <= touchPointMapIndex.y ||
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
    
    auto pGameLogNode = getChildByTag(RogueScene::kGameLogTag);
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
        pItemWindowLayer->setItemDropMenuCallback([this](Object* pSender, DropItemSprite::DropItemDto dropItemDto) {
            CCLOG("RogueScene::itemDropMenuCallback");
            
            auto pPlayerSprite = getPlayerActorSprite(1);
            
            // すでにアイテムが置いてある場合は置けない
            if (m_mapManager.getDropMapItem(&pPlayerSprite->getActorMapItem()->mapIndex)->mapDataType != MapDataType::NONE)
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
                    
                    if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_WEAPON)
                    {
                        pPlayerSprite->equipReleaseWeapon();
                    }
                    else if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_ACCESSORY)
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
        
        pItemWindowLayer->setItemUseMenuCallback([this](Object* pSender, DropItemSprite::DropItemDto dropItemDto) {
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
        
        pItemWindowLayer->setItemEquipMenuCallback([this](Object* pSender, DropItemSprite::DropItemDto dropItemDto) {
            CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", dropItemDto.itemType);
            
            auto pPlayerSprite = getPlayerActorSprite(1);
            
            if (dropItemDto.isEquip)
            {
                if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_WEAPON)
                {
                    // 武器装備
                    pPlayerSprite->getActorDto()->equip.weaponObjectId = dropItemDto.objectId;
                    pPlayerSprite->getActorDto()->equip.weaponId       = dropItemDto.itemId;
                    pPlayerSprite->getActorDto()->equip.weaponImgResId = dropItemDto.imageResId;
                    pPlayerSprite->getActorDto()->equip.weaponName     = dropItemDto.name;
                    pPlayerSprite->getActorDto()->equip.weaponStr      = 5; // TODO: valueはマスタから実はnameも取りたい・・・
                }
                else if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_ACCESSORY)
                {
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
                if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_WEAPON)
                {
                    // 武器解除
                    pPlayerSprite->equipReleaseWeapon();
                }
                else if (dropItemDto.itemType == DropItemSprite::ItemType::EQUIP_ACCESSORY)
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
        
        // 閉じるボタン
        auto pCloseMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("とじる", GAME_FONT(10), 10), Size(12, 4), [this](Object *pSender) {
            // 閉じる
            this->hideItemList();
        });
        pCloseMenuItemLabel->setPosition(Point(pItemWindowLayer->getContentSize().width, pCloseMenuItemLabel->getPositionY() -  pCloseMenuItemLabel->getContentSize().height / 2));
        
        auto pMenu = Menu::create(pCloseMenuItemLabel, NULL);
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
    if (m_mapManager.getActorMapItem(&mapIndex)->mapDataType != MapDataType::NONE)
    {
        return false;
    }
    auto pTileMapLayer = getChildByTag(RogueScene::kTiledMapTag);
    auto pEnemyLayer = pTileMapLayer->getChildByTag(RogueScene::kTiledMapEnemyBaseTag);
    // 出現数のカウントをseqNoにする
    long enemyCount = m_enemyCount;
    m_enemyCount++;
    
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
    pEnemyLayer->addChild(enemySprite, RogueScene::zTiledMapEnemyBaseIndex, (RogueScene::kTiledMapEnemyBaseTag + enemyMapItem.seqNo));
    
    // マップに追加
    m_mapManager.addActor(enemySprite->getActorMapItem());
    
    // ミニマップも更新
    addMiniMapItem(enemySprite->getActorMapItem(), enemySprite->getTag());
    
    return true;
}

void RogueScene::removeEnemyActorSprite(ActorSprite* pEnemySprite)
{
    auto pEnemyMapLayer = getChildByTag(RogueScene::kTiledMapTag)->getChildByTag(RogueScene::TiledMapTag::kTiledMapEnemyBaseTag);
    
    // mapManagerから消す
    m_mapManager.removeMapItem(pEnemySprite->getActorMapItem());
    
    // ミニマップを更新
    getGridSpriteBatchNode()->removeChildByTag(pEnemySprite->getTag());
    
    // Map上からremoveする
    pEnemyMapLayer->removeChild(pEnemySprite);
}

bool RogueScene::tileSetDropMapItem(DropItemSprite::DropItemDto dropItemDto, MapIndex mapIndex)
{
    // すでにアイテムが置いてある場合は置けない
    if (m_mapManager.getDropMapItem(&mapIndex)->mapDataType != MapDataType::NONE)
    {
        return false;
    }
    
    auto pTileMapLayer = getChildByTag(RogueScene::kTiledMapTag);
    auto pDropItemLayer = pTileMapLayer->getChildByTag(RogueScene::kTiledMapDropItemBaseTag);
    
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
    pDropItemLayer->addChild(pDropItemSprite, RogueScene::zTiledMapDropItemBaseIndex, RogueScene::kTiledMapDropItemBaseTag + dropMapItem.seqNo);
    
    // マップに追加
    m_mapManager.addDropItem(pDropItemSprite->getDropMapItem());
    
    // ミニマップも更新
    addMiniMapItem(pDropItemSprite->getDropMapItem(), pDropItemSprite->getTag());
    
    return true;
}

void RogueScene::removeDropItemSprite(Node* pRemoveParentNode, DropItemSprite* pDropItemSprite)
{
    // mapManagerから消す
    m_mapManager.removeMapItem(pDropItemSprite->getDropMapItem());
    
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
        auto pBatchNode = static_cast<SpriteBatchNode*>(pMiniMapLayer->getChildByTag(RogueScene::MiniMapSimbolBatchNodeTag));
        if (!pBatchNode)
        {
            pBatchNode = SpriteBatchNode::create("grid32.png");
            pBatchNode->setTag(RogueScene::MiniMapSimbolBatchNodeTag);
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
        float scale = 1.0f / 8.0f;
        pSprite->setScale(scale);
        pSprite->setContentSize(m_baseTileSize / 8);
        // 現在位置からPositionを取得して1/8にする
        auto point = indexToPointNotTileSize(pMapItem->mapIndex) / 8;
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
    auto pStatusBarLayer = getChildByTag(RogueScene::kStatusBarTag);
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
        int floor = m_questId; // フロア情報（クエストID=フロア数でいい？)
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
#pragma mark マップ座標変換

Point RogueScene::indexToPoint(MapIndex mapIndex)
{
    return indexToPoint(mapIndex.x, mapIndex.y);
}

Point RogueScene::indexToPoint(int mapIndex_x, int mapIndex_y)
{
    // タイルサイズを考慮
    return Point(m_baseTileSize.width * mapIndex_x + m_baseTileSize.width * 0.5, m_baseTileSize.height * mapIndex_y + m_baseTileSize.height * 0.5);
}

MapIndex RogueScene::pointToIndex(Point point)
{
    // タイルサイズを考慮
    point.x = point.x - m_baseTileSize.width * 0.5;
    point.y = point.y - m_baseTileSize.height * 0.5;
    return touchPointToIndex(point);
}

MapIndex RogueScene::touchPointToIndex(Point point)
{
    MapIndex mapIndex;
    mapIndex.x = point.x / m_baseTileSize.width;
    mapIndex.y = point.y / m_baseTileSize.height;
    return mapIndex;
}

Point RogueScene::indexToPointNotTileSize(MapIndex mapIndex)
{
    return indexToPointNotTileSize(mapIndex.x, mapIndex.y);
}

Point RogueScene::indexToPointNotTileSize(int mapIndex_x, int mapIndex_y)
{
    return Point(m_baseTileSize.width * mapIndex_x, m_baseTileSize.height * mapIndex_y);
}

MapIndex RogueScene::mapIndexToTileIndex(MapIndex mapIndex)
{
    MapIndex tileIndex;
    tileIndex.x = mapIndex.x;
    tileIndex.y = m_baseMapSize.height - mapIndex.y - 1;
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
    auto pEnemyMapLayer = getChildByTag(RogueScene::kTiledMapTag)->getChildByTag(RogueScene::TiledMapTag::kTiledMapEnemyBaseTag);
    return static_cast<ActorSprite*>(pEnemyMapLayer->getChildByTag(RogueScene::TiledMapTag::kTiledMapEnemyBaseTag + seqNo));
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

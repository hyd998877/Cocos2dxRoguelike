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

#include "MLevelDao.h"
#include "MWeaponDao.h"
#include "MUseItemDao.h"
#include "MAccessoryDao.h"

#include "AccountData.h"

#include "MypageScene.h"

// プロトタイプ宣言
//int GetRandom(int min,int max);

std::size_t f_r(const std::string& s, char c)
{
    std::string::size_type pos = s.find(c);
    return (pos == std::string::npos) ? 0 : (1 + f_r(s.substr(pos+1), c));
}

#pragma mark
#pragma mark main

RogueScene::RogueScene()
:rogue_play_data_()
//base_map_size_(Size::ZERO),
//base_tile_size_(Size::ZERO),
//base_content_size_(Size::ZERO)
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

bool RogueScene::initWithQuestId(int quest_id) {
    
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    // 乱数初期化
    srand((unsigned int)time(NULL));

    // データロード
    AccountData::getInstance()->load();
    
    ActorSprite::ActorDto actor_dto;
    // 不一致の場合初期化
    if (AccountData::getInstance()->rogue_play_data_.quest_id != quest_id) {
        AccountData::getInstance()->reset();
        rogue_play_data_.quest_id = quest_id;
        
        actor_dto.name = "ジニー";
        actor_dto.faceImgId = 0;
        actor_dto.imageResId = 1015;
        // 基本
        actor_dto.attackRange = 1;
        actor_dto.movePoint = 5;
        actor_dto.playerId = 4; // actorId
        // 攻守
        actor_dto.attackPoint = 5;
        actor_dto.defencePoint = 1;
        // 経験値
        actor_dto.exp = 0;
        actor_dto.nextExp = 10;
        // HP
        actor_dto.hitPoint = 15;
        actor_dto.hitPointLimit = 15;
        actor_dto.lv = 1;
        // 満腹度？精神力？
        actor_dto.magicPoint = 100;
        actor_dto.magicPointLimit = 100;
        // 装備
        actor_dto.equip = ActorSprite::createEquipDto();
        // お金
        actor_dto.gold = 0;
    } else {
        // ロード処理
        rogue_play_data_ = AccountData::getInstance()->rogue_play_data_;
        actor_dto = AccountData::getInstance()->player_actor_;
    }
    
    auto win_size = Director::getInstance()->getWinSize();

    // ---------------------
    // フロア開始カットイン表示
    // ---------------------
    this->addChild(createFloorTitleCutinLayer(rogue_play_data_.quest_id), RogueScene::CutInLayerZOrder);
    
    // ---------------------
    // タイルマップを生成
    // ---------------------
    // TODO: (kyokomi)ランダムなマップIDを指定する
    rogue_play_data_.floor_id = 2;
    
    auto tiled_map_layer = RogueTMXTiledMap::create(StringUtils::format("tmx/quest_%d.tmx", rogue_play_data_.floor_id));

    tiled_map_layer->setPosition(Point::ZERO);
    this->addChild(tiled_map_layer, RogueScene::TiledMapLayerZOrder, RogueScene::TiledMapLayerTag);
    
    //-------------------------
    // ステータスバー？
    //-------------------------
    auto statusLayer = LayerColor::create(Color4B::BLACK);
    statusLayer->setContentSize(Size(win_size.width, win_size.height / 7));
    statusLayer->setPosition(Point(0, win_size.height - statusLayer->getContentSize().height));
    
    // 更新する
    auto sampleText = LabelTTF::create(" --F Lv-- HP ---/--- 満腹度 ---/---          - G", GAME_FONT(32), GAME_FONT_SIZE(32));
    
    sampleText->setPosition(Point(sampleText->getContentSize().width / 2, statusLayer->getContentSize().height / 2));
    statusLayer->addChild(sampleText);
    
    this->addChild(statusLayer, RogueScene::StatusBarLayerZOrder, RogueScene::StatusBarLayerTag);
    
    //-------------------------
    // ゲームログ表示
    //-------------------------
    auto pGameLogLayer = LayerColor::create(Color4B(0, 0, 0, 192));
    pGameLogLayer->setContentSize(Size(win_size.width * 0.5, win_size.height / 5));
    pGameLogLayer->setPosition(win_size.width / 2 - pGameLogLayer->getContentSize().width / 2, 0);
    
    CommonWindowUtil::attachWindowWaku(pGameLogLayer);
    
    int baseFontSize = 20;
    auto pLogTextLabel = LabelTTF::create("", GAME_FONT(baseFontSize), GAME_FONT_SIZE(baseFontSize), Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
    pLogTextLabel->setPosition(Point(pLogTextLabel->getContentSize().width / 2 + pLogTextLabel->getFontSize() / 4, pGameLogLayer->getContentSize().height - pLogTextLabel->getContentSize().height / 2 - pLogTextLabel->getFontSize() / 4));
    pGameLogLayer->addChild(pLogTextLabel);
    
    this->addChild(pGameLogLayer, RogueScene::GameLogLayerZOrder, RogueScene::GameLogLayerTag);
    
    // ------------------------
    // ミニマップ
    // ------------------------
    auto mini_map_layer = tiled_map_layer->getMiniMapLayer();
    // ステータスバーの下くらい
    mini_map_layer->setPosition(0, mini_map_layer->getPositionY() + win_size.height - mini_map_layer->getContentSize().height - statusLayer->getContentSize().height);
    this->addChild(mini_map_layer, RogueScene::MiniMapLayerZOrder, RogueScene::MiniMapLayerTag);
    
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
    MapIndex playerRandMapIndex = tiled_map_layer->getRandomMapIndex(false, true);
    playerRandMapIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
    actorMapItem.mapIndex = playerRandMapIndex;
    actorMapItem.seqNo = 1;
    actorMapItem.moveDist = actor_dto.movePoint;
    actorMapItem.attackDist = actor_dto.attackRange;
    actorMapItem.moveDone = false;
    actorMapItem.attackDone = false;
    
    auto actorSprite = ActorSprite::createWithActorDto(actor_dto);
    MapIndex baseActorIndex = tiled_map_layer->pointToIndex(Point(win_size.width/2, win_size.height/2));
    actorSprite->setPosition(tiled_map_layer->indexToPoint(baseActorIndex));
    actorSprite->setActorMapItem(actorMapItem);
    actorSprite->runBottomAction();
    // プレイヤーは画面中心にくるのでmapLayerに追加しない
    this->addChild(actorSprite, RogueScene::ActorPlayerZOrder, (RogueScene::ActorPlayerTag + actorMapItem.seqNo));

    // マップに追加
    MapManager::getInstance()->addActor(actorSprite->getActorMapItem());

    refreshStatus();
    
    // プレイヤーの位置表示用（同じく1/8サイズ）
    tiled_map_layer->addMiniMapItem(actorSprite->getActorMapItem(), actorSprite->getTag());
    
    // プレイヤー位置にマップを移動
    MapIndex moveIndex = {
        playerRandMapIndex.x - baseActorIndex.x,
        playerRandMapIndex.y - baseActorIndex.y,
        actorMapItem.mapIndex.moveDictType
    };
    // 移動する距離をPointに変換
    auto addMovePoint = Point(tiled_map_layer->getTileSize().width * moveIndex.x, tiled_map_layer->getTileSize().height * moveIndex.y);
    // 移動
    tiled_map_layer->runAction(MoveTo::create(0.0f, tiled_map_layer->getPosition() - addMovePoint));
    
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
    
    MapIndex enemyMapIndex1 = tiled_map_layer->getRandomMapIndex(false, true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemyDto, enemyMapIndex1);
    
    ActorSprite::ActorDto enemyDto2 = enemyDto;
    MapIndex enemyMapIndex2 = tiled_map_layer->getRandomMapIndex(false, true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemyDto2, enemyMapIndex2);
    
    ActorSprite::ActorDto enemyDto3 = enemyDto;
    MapIndex enemyMapIndex3 = tiled_map_layer->getRandomMapIndex(false, true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemyDto3, enemyMapIndex3);
    
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
        
        MapIndex mapIndex = tiled_map_layer->getRandomMapIndex(false, false);
        tiled_map_layer->tileSetDropMapItem(dropItemDto, mapIndex);

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
        
        MapIndex mapIndex3 = tiled_map_layer->getRandomMapIndex(false, false);;
        tiled_map_layer->tileSetDropMapItem(dropItemDto3, mapIndex3);
        
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
        
        MapIndex mapIndex4 = tiled_map_layer->getRandomMapIndex(false, false);;
        tiled_map_layer->tileSetDropMapItem(dropItemDto4, mapIndex4);
        
        object_count++;
    }
    
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
    rogueMapLighting();
    
    return true;
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
    
    auto rogue_map_layer = getRogueMapLayer();
    
    auto pMenuKeyUp = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyUpが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = getRogueMapLayer()->pointToIndex(point);
            this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x, mapIndex.y + 1));
        }
    });
    
    pMenuKeyUp->setPosition(rogue_map_layer->indexToPoint(1, 2));
    resultArray.pushBack(pMenuKeyUp);
    
    auto pMenuKeyRight = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyRightが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = getRogueMapLayer()->pointToIndex(point);
            this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x + 1, mapIndex.y));
        }
    });
    pMenuKeyRight->setRotation(90.0f);
    pMenuKeyRight->setPosition(rogue_map_layer->indexToPoint(2, 1));
    resultArray.pushBack(pMenuKeyRight);
    
    auto pMenuKeyDown = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyDownが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = getRogueMapLayer()->pointToIndex(point);
            this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x, mapIndex.y - 1));
        }
    });
    pMenuKeyDown->setRotation(180.0f);
    pMenuKeyDown->setPosition(rogue_map_layer->indexToPoint(1, 0));
    resultArray.pushBack(pMenuKeyDown);
    
    auto pMenuKeyLeft = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyLeftが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN)
        {
            auto winSize = Director::getInstance()->getWinSize();
            Point point = Point(winSize.width / 2, winSize.height / 2);
            MapIndex mapIndex = getRogueMapLayer()->pointToIndex(point);
            this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x - 1, mapIndex.y));
        }
    });
    pMenuKeyLeft->setRotation(270.0f);
    pMenuKeyLeft->setPosition(rogue_map_layer->indexToPoint(0, 1));
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
    auto rogu_map_layer = getRogueMapLayer();
    Size base_tile_size = rogu_map_layer->getTileSize();
    
    pA_MenuButton->setPosition(Point(win_size.width - base_tile_size.width, rogu_map_layer->indexToPoint(12, 1).y));
    pA_MenuButton->setTag(RogueScene::A_ButtonMenuTag);
    resultArray.pushBack(pA_MenuButton);
    
    auto b_button = Sprite::create("ui/b_button.png");
    auto b_buttonPress = Sprite::create("ui/b_button_press.png");
    b_buttonPress->setOpacity(128);
    auto pB_MenuButton = MenuItemSprite::create(b_button, b_buttonPress, [this](Ref* pSender) {
        CCLOG("Bボタンが押された！");
    });
    pB_MenuButton->setPosition(Point(win_size.width - base_tile_size.width * 2, rogu_map_layer->indexToPoint(11, 0).y));
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
    pC_MenuButton->setPosition(Point(win_size.width - base_tile_size.width * 3, rogu_map_layer->indexToPoint(10, 1).y));
    pC_MenuButton->setTag(RogueScene::C_ButtonMenuTag);
    resultArray.pushBack(pC_MenuButton);
    
    auto d_button = Sprite::create("ui/d_button.png");
    auto d_buttonPress = Sprite::create("ui/d_button_press.png");
    d_buttonPress->setOpacity(128);
    auto pD_MenuButton = MenuItemSprite::create(d_button, d_buttonPress, [this](Ref* pSender) {
        CCLOG("Dボタンが押された！");
        showItemList(1);
    });
    pD_MenuButton->setPosition(Point(win_size.width - base_tile_size.width * 2, rogu_map_layer->indexToPoint(11, 2).y));
    pD_MenuButton->setTag(RogueScene::D_ButtonMenuTag);
    resultArray.pushBack(pD_MenuButton);
    
    return resultArray;
}

// floorTitleカットインを生成する
// タッチイベントでフェードアウトしてremoveする
// private
LayerColor* RogueScene::createFloorTitleCutinLayer(int quest_id) {
    auto win_size = Director::getInstance()->getWinSize();
    const int floor_title_font_size = 47;
    
    // 真っ黒の全画面で中心にフロア名 N層 と表示され　タップするとフェードアウトして消える
    auto floor_title_cutin_layer = LayerColor::create(Color4B::BLACK);
    floor_title_cutin_layer->setContentSize(win_size);
    // テキスト中央
    // TODO: (kyokomi) タイトルはdaoからとってくる予定
    auto floor_title_text = StringUtils::format("初心者の洞窟 %d層", quest_id);
    auto floor_title_text_label = LabelTTF::create(floor_title_text, GAME_FONT(floor_title_font_size), GAME_FONT_SIZE(floor_title_font_size));
    floor_title_text_label->setPosition(Point(floor_title_cutin_layer->getContentSize().width / 2, floor_title_cutin_layer->getContentSize().height / 2));
    floor_title_cutin_layer->addChild(floor_title_text_label);

    auto cutin_listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    cutin_listener->setSwallowTouches(true);
    cutin_listener->onTouchBegan = [floor_title_cutin_layer](Touch* touch, Event* event) -> bool {
        // カットインを破棄
        floor_title_cutin_layer->runAction(Sequence::create(FadeOut::create(1.0f), CallFunc::create([floor_title_cutin_layer]() {
            floor_title_cutin_layer->setVisible(false);
            floor_title_cutin_layer->removeAllChildrenWithCleanup(true);
        }), NULL));
        return false;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cutin_listener, this);
    
    return floor_title_cutin_layer;
}

void RogueScene::onEnter() {
    CCLOG("%s(%d) onEnter", __FILE__, __LINE__);
    Layer::onEnter();
}

void RogueScene::onEnterTransitionDidFinish() {
    CCLOG("%s(%d) onEnterTransitionDidFinish", __FILE__, __LINE__);
    Layer::onEnterTransitionDidFinish();
}

#pragma mark
#pragma mark ゲームステート関連

void RogueScene::changeGameStatus(GameStatus gameStatus)
{
    CCLOG("turn %d change gameStatus %d => %d", rogue_play_data_.turn_count, rogue_play_data_.game_status, gameStatus);
    
    auto rogue_map_layer = getRogueMapLayer();
    auto pPlayer = getPlayerActorSprite(1);
    
    GameStatus beforeGameStatus = rogue_play_data_.game_status;
    rogue_play_data_.game_status = gameStatus;
    
    if (rogue_play_data_.game_status == GameStatus::GAME_OVER)
    {        
        // TODO: ゲームオーバーの演出
        
        // TODO: ゲームオーバー画面Scene？表示
        
        // TODO: とりあえずタイトルへ
        this->changeScene(MypageScene::scene());
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
            MapIndex rePopIndex = rogue_map_layer->getRandomMapIndex(false, true);
            
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
            rogue_map_layer->tileSetEnemyActorMapItem(enemyDto, rePopIndex);
        }
    }
    
    // プレイステータス更新
    refreshStatus();
    // 照明情報更新
    rogueMapLighting();
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

#pragma mark
#pragma mark モンスターのAI関連

void RogueScene::enemyTurn()
{
    auto rogue_map_layer = getRogueMapLayer();
    
    // モンスターの数だけ繰り返す
    std::list<ActorMapItem> enemyList = MapManager::getInstance()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList)
    {
        // ランダムでとどまるか移動するかきめる
        int rand = GetRandom(2, 2);
        if (rand == 1)
        {
            auto pEnemySprite = rogue_map_layer->getEnemyActorSprite(enemyMapItem.seqNo);
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
            auto pEnemySprite = rogue_map_layer->getEnemyActorSprite(enemyMapItem.seqNo);
            MapIndex addMoveIndex = {
                moveMapIndex.x - pEnemySprite->getActorMapItem()->mapIndex.x,
                moveMapIndex.y - pEnemySprite->getActorMapItem()->mapIndex.y,
                MapManager::getInstance()->checkMoveDirectionType(moveMapIndex, pEnemySprite->getActorMapItem()->mapIndex)
            };
            pEnemySprite->runMoveAction(addMoveIndex);
            // 行動前にする
            pEnemySprite->getActorMapItem()->moveDone = false;
            pEnemySprite->getActorMapItem()->attackDone = false;
            
            if (rogue_map_layer->isMapLayerOver(moveMapIndex))
            {
                CCLOG("移動不可 seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->getActorMapItem()->moveDone = true;
            }
            else if (rogue_map_layer->isTiledMapColisionLayer(moveMapIndex))
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
                auto pMove1 = MoveTo::create(speed, pEnemySprite->getPosition() + rogue_map_layer->indexToPointNotTileSize(addMoveIndex));
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
                rogue_map_layer->moveEnemyMap(enemyMapItem.seqNo, addMoveIndex, getAnimationSpeed(),CallFunc::create([this, pEnemySprite]() {
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
        auto pEnemySprite = getRogueMapLayer()->getEnemyActorSprite(enemyMapItem.seqNo);
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

void RogueScene::touchEventExec(cocos2d::Point touchPoint)
{
    auto rogue_map_layer = getRogueMapLayer();
    // マップ移動分を考慮
    MapIndex touchPointMapIndex = rogue_map_layer->touchPointToIndex(touchPoint - rogue_map_layer->getPosition());
    CCLOG("touchEventExec touchPointMapIndex x = %d y = %d", touchPointMapIndex.x, touchPointMapIndex.y);
    
    // 画面外判定
    if (rogue_map_layer->isMapLayerOver(touchPointMapIndex))
    {
        // タッチ判定とみなさない
        CCLOG("画面外");
        return;
    }
    // タッチした位置が有効なIndexか判定
    MapIndex addMoveIndex = checkTouchEventIndex(touchPointMapIndex);
    if (addMoveIndex.x == 0 && addMoveIndex.y == 0)
    {
        // タッチ判定とみなさない
        CCLOG("有効じゃない");
        return;
    }
    
    touchEventExec(addMoveIndex, touchPointMapIndex);
}

void RogueScene::touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex)
{
    auto rogue_map_layer = getRogueMapLayer();
    
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
        if (rogue_map_layer->isTiledMapColisionLayer(touchPointMapIndex))
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
                auto pDropItemSprite = rogue_map_layer->getDropItemSprite(pDropMapItem->seqNo);
                
                // メッセージログ
                auto pDropItemDto = pDropItemSprite->getDropItemDto();
                logMessage("%sを拾った。", pDropItemDto->name.c_str());
                
                // イベントリに追加する
                getItemWindowLayer()->addItemList(*pDropItemDto);
                
                // Map上から削除する
                rogue_map_layer->removeDropItemSprite(pDropItemSprite);
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
            rogue_map_layer->movePlayerMap(pActorSprite, addMoveIndex, getAnimationSpeed(), CallFunc::create([this](void) {
                // ターンエンド
                changeGameStatus(GameStatus::ENEMY_TURN);
            }));
            
            // コールバックまでgameStatusを更新はしない
        }
    }
}

void RogueScene::attack()
{
    auto rogue_map_layer = getRogueMapLayer();
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
    auto pMove1 = MoveTo::create(speed, pActorSprite->getPosition() + rogue_map_layer->indexToPointNotTileSize(addMapIndex.x, addMapIndex.y));
    auto pMove2 = MoveTo::create(speed, pActorSprite->getPosition());

    ActorSprite* pEnemySprite = NULL;
    // 敵をタッチした
    auto pEnemyMapItem = MapManager::getInstance()->getActorMapItem(&attackMapIndex);
    if (pEnemyMapItem->mapDataType == MapDataType::ENEMY)
    {
        pEnemySprite = rogue_map_layer->getEnemyActorSprite(pEnemyMapItem->seqNo);
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
                getRogueMapLayer()->removeEnemyActorSprite(pEnemySprite);
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
                if (this->getRogueMapLayer()->tileSetDropMapItem(dropItemDto, pPlayerSprite->getActorMapItem()->mapIndex))
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
        // 作成
        auto pStr = String::createWithFormat(" %2dF Lv%3d HP %3d/%3d 満腹度 %d/%d %10d G", floor, pPlayerDto->lv, pPlayerDto->hitPoint, pPlayerDto->hitPointLimit, pPlayerDto->magicPoint, pPlayerDto->magicPointLimit, pPlayerDto->gold);
        
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

void RogueScene::rogueMapLighting() {
    
    auto actor_sprite = getPlayerActorSprite(1);
    auto rogue_map_layer = getRogueMapLayer();
    
    // プレイヤー周辺３＊３はかならず更新
    // プレイヤー視野でマップをリフレッシュする
    rogue_map_layer->refreshPlayerRectAutoMapping(actor_sprite->getActorMapItem()->mapIndex);
    
    Rect floorInfoIndexRect = rogue_map_layer->getTileMapFloorInfo(actor_sprite->getActorMapItem()->mapIndex);
    if (floorInfoIndexRect.equals(Rect::ZERO)) {
        // 視野をプレイヤー周辺に更新
        rogue_map_layer->hideFloorLighting();
        showPlayerLighting(actor_sprite);
    } else {
        // 視野を部屋に更新
        hidePlayerLighting();
        
        rogue_map_layer->showFloorLighting(actor_sprite->getActorMapItem()->mapIndex, floorInfoIndexRect);
        
        // マッピングとマップ情報も更新
        rogue_map_layer->refreshFloorRectAutoMapping(floorInfoIndexRect);
    }
}

void RogueScene::showPlayerLighting(ActorSprite* actor_sprite) {
    
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
        light_draw = DrawNode::create();
        light_draw->drawDot(actor_sprite->getPosition(), getRogueMapLayer()->getTileSize().width * 3.0f / 2.0f, Color4F::WHITE);
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

#pragma mark
#pragma mark 汎用

ActorSprite* RogueScene::getPlayerActorSprite(int seqNo)
{
    return static_cast<ActorSprite*>(getChildByTag(RogueScene::ActorPlayerTag + seqNo));
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

RogueTMXTiledMap* RogueScene::getRogueMapLayer() {
    return static_cast<RogueTMXTiledMap*>(this->getChildByTag(RogueScene::TiledMapLayerTag));
}


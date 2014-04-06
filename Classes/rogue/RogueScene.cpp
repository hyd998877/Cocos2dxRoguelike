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
#include "MMonster.h"
#include "MPlayer.h"

#include "AccountData.h"

#include "MypageScene.h"

// プロトタイプ宣言
std::size_t f_r(const std::string& s, char c)
{
    std::string::size_type pos = s.find(c);
    return (pos == std::string::npos) ? 0 : (1 + f_r(s.substr(pos+1), c));
}

#pragma mark
#pragma mark main

RogueScene::RogueScene()
:rogue_play_data_()
{
    CCLOG("new rogueScene");
}

RogueScene::~RogueScene()
{
    CCLOG("death rogueScene");
}

Scene* RogueScene::scene(int questId) {
    Scene *scene = Scene::create();
    RogueScene *layer = RogueScene::createWithQuestId(questId);
    scene->addChild(layer);
    return scene;
}

RogueScene* RogueScene::createWithQuestId(int questId) {
    auto *pRet = new RogueScene();
    if (pRet && pRet->initWithQuestId(questId)) {
        pRet->autorelease();
        return pRet;
    } else {
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
        // デフォルトステータス
        actor_dto = ActorSprite::createActorDto(m_player::data_.at("1").asString());
        actor_dto.equip = ActorSprite::createEquipDto();
    } else {
        // ロード処理
        rogue_play_data_ = AccountData::getInstance()->rogue_play_data_;
        actor_dto = AccountData::getInstance()->player_actor_;
    }
    
    auto win_size = Director::getInstance()->getWinSize();

    // ---------------------
    // フロア開始カットイン表示
    // ---------------------
    this->addChild(createFloorTitleCutInLayer(rogue_play_data_.quest_id), RogueScene::CutInLayerZOrder);
    
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
    const float StatusBarFontSize = 32;
    auto statusLayer = LayerColor::create(Color4B::BLACK);
    statusLayer->setContentSize(Size(win_size.width, win_size.height / 7));
    statusLayer->setPosition(Point(0, win_size.height - statusLayer->getContentSize().height));
    
    // あとで更新するやつ
    auto sampleText = Label::create(" --F Lv-- HP ---/--- 満腹度 ---/---          - G", GAME_FONT(StatusBarFontSize), GAME_FONT_SIZE(StatusBarFontSize));
    
    sampleText->setPosition(Point(sampleText->getContentSize().width / 2, statusLayer->getContentSize().height / 2));
    statusLayer->addChild(sampleText);
    
    // 装備情報（武器）ステータスバーの下
    int attackPoint = actor_dto.attackPoint;
    if (actor_dto.equip.weaponId > 0) {
        attackPoint += actor_dto.equip.weaponStr;
    }
    std::string attackPointText = StringUtils::format("%3d", attackPoint);
    auto equipWeaponLayer = CommonWindowUtil::createSpriteWithLabelLayer(Size(win_size.width / 7, win_size.height / 8), "grid32.png", attackPointText, GAME_FONT(StatusBarFontSize), GAME_FONT_SIZE(StatusBarFontSize));
    // 2つあるうちの左側なので2かけてます
    {
        equipWeaponLayer->getChildByTag(1)->setVisible(false);
        float pointX = statusLayer->getContentSize().width - (equipWeaponLayer->getContentSize().width * 2);
        float pointY = (statusLayer->getContentSize().height / 2 + equipWeaponLayer->getContentSize().height / 2) * -1;
        equipWeaponLayer->setPosition(cocos2d::Point(pointX, pointY));
        equipWeaponLayer->setTag(RogueScene::StatusBarEquipWeaponTag);
        statusLayer->addChild(equipWeaponLayer);
    }
    
    // 装備情報（防具）ステータスバーの下
    int defencePoint = actor_dto.defencePoint;
    if (actor_dto.equip.accessoryId > 0) {
        defencePoint += actor_dto.equip.accessoryDef;
    }
    std::string defencePointText = StringUtils::format("%3d", defencePoint);
    auto equipAccessoryLayer = CommonWindowUtil::createSpriteWithLabelLayer(Size(win_size.width / 7, win_size.height / 8), "grid32.png", defencePointText, GAME_FONT(StatusBarFontSize), GAME_FONT_SIZE(StatusBarFontSize));
    {
        equipAccessoryLayer->getChildByTag(1)->setVisible(false);
        float pointX = statusLayer->getContentSize().width - equipAccessoryLayer->getContentSize().width;
        float pointY = (statusLayer->getContentSize().height / 2 + equipAccessoryLayer->getContentSize().height / 2) * -1;
        equipAccessoryLayer->setPosition(cocos2d::Point(pointX, pointY));
        equipAccessoryLayer->setTag(RogueScene::StatusBarEquipAccessoryTag);
        statusLayer->addChild(equipAccessoryLayer);
    }
    
    this->addChild(statusLayer, RogueScene::StatusBarLayerZOrder, RogueScene::StatusBarLayerTag);
    
    //-------------------------
    // ゲームログ表示
    //-------------------------
    auto pGameLogLayer = LayerColor::create(Color4B(0, 0, 0, 192));
    pGameLogLayer->setContentSize(Size(win_size.width * 0.5, win_size.height * 0.2));
    pGameLogLayer->setPosition(win_size.width / 2 - pGameLogLayer->getContentSize().width / 2, 0);
    
    CommonWindowUtil::attachWindowWaku(pGameLogLayer);
    
    int baseFontSize = 20;
    auto pLogTextLabel = Label::create("", GAME_FONT(baseFontSize), GAME_FONT_SIZE(baseFontSize), Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
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
    showItemList();
    hideItemList();
    // アイテム引き継ぎ
    getItemWindowLayer()->setItemList(AccountData::getInstance()->item_list_);
    
    // ---------------------
    // プレイヤー生成
    // ---------------------
    auto actor_sprite = ActorSprite::createWithActorDto(actor_dto);
    // 画面の中心に配置
    MapIndex base_actor_index = tiled_map_layer->pointToIndex(Point(win_size.width/2, win_size.height/2));
    base_actor_index.moveDictType = MoveDirectionType::MOVE_DOWN;
    // マップ上は、ランダムな位置で開始する
    ActorMapItem actor_map_item = tiled_map_layer->startPlayerRandomPosition(actor_dto, base_actor_index);
    actor_sprite->setActorMapItem(actor_map_item);
    actor_sprite->setPosition(tiled_map_layer->indexToPoint(base_actor_index));
    actor_sprite->runBottomAction();
    // プレイヤーは画面中心にくるのでmapLayerに追加しない
    this->addChild(actor_sprite, RogueScene::ActorPlayerZOrder, (RogueScene::ActorPlayerTag + actor_map_item.seqNo));
    // マップに追加
    tiled_map_layer->setPlayerActorMapItem(actor_sprite->getActorMapItem(), actor_sprite->getTag());
    
    refreshStatusEquip(actor_dto);
    // ---------------------
    // 敵キャラ生成
    // ---------------------
    ActorSprite::ActorDto enemy_dto1 = ActorSprite::createActorDto(m_monster::data_.at("1").asString());
    enemy_dto1.equip = ActorSprite::createEquipDto();
    ActorSprite::ActorDto enemy_dto2 = ActorSprite::createActorDto(m_monster::data_.at("2").asString());
    enemy_dto2.equip = ActorSprite::createEquipDto();
    ActorSprite::ActorDto enemy_dto3 = ActorSprite::createActorDto(m_monster::data_.at("3").asString());
    enemy_dto3.equip = ActorSprite::createEquipDto();
    
    MapIndex enemyMapIndex1 = tiled_map_layer->getFloorRandomMapIndex(true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemy_dto1, enemyMapIndex1);
    
    MapIndex enemyMapIndex2 = tiled_map_layer->getFloorRandomMapIndex(true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemy_dto2, enemyMapIndex2);
    
    MapIndex enemyMapIndex3 = tiled_map_layer->getFloorRandomMapIndex(true);
    tiled_map_layer->tileSetEnemyActorMapItem(enemy_dto3, enemyMapIndex3);
    
    //-------------------------
    // アイテム配置
    //-------------------------
    int object_count = AccountData::getInstance()->rogue_play_data_.item_count;
    if (object_count < 0) {
        object_count = 0;
    }
    // TODO: (kyokomi) とりあえずアイテムとか武器はテストのため全種類配置してる
    for (int i = 0; i < 7; i++) {
        MUseItem mUseItem = MUseItemDao::getInstance()->selectById(i + 1);
        DropItemSprite::DropItemDto dropItemDto;
        dropItemDto.objectId = object_count + i + 1; // 単純に連番でいい
        dropItemDto.itemId = mUseItem.getUseItemId();
        dropItemDto.itemType = mUseItem.getUseItemType();
        dropItemDto.imageResId = mUseItem.getUseItemImageId();
        dropItemDto.name = mUseItem.getUseItemName();
        dropItemDto.isEquip = false;
        
        MapIndex mapIndex = tiled_map_layer->getFloorRandomMapIndex(false);
        tiled_map_layer->tileSetDropMapItem(dropItemDto, mapIndex);

        object_count++;
    }

    for (int i = 0; i < 11; i++) {
        MWeapon mWeapon = MWeaponDao::getInstance()->selectById(i + 1);
        DropItemSprite::DropItemDto dropItemDto3;
        dropItemDto3.objectId = object_count + i + 1;
        dropItemDto3.itemId = mWeapon.getWeaponId(); // weaponId
        dropItemDto3.itemType = MUseItem::ItemType::EQUIP_WEAPON;
        dropItemDto3.imageResId = mWeapon.getWeaponImageId();
        dropItemDto3.name = mWeapon.getWeaponName();
        dropItemDto3.isEquip = false;
        
        MapIndex mapIndex3 = tiled_map_layer->getFloorRandomMapIndex(false);
        tiled_map_layer->tileSetDropMapItem(dropItemDto3, mapIndex3);
        
        object_count++;
    }
    for (int i = 0; i < 7; i++) {
        MAccessory mAccessory = MAccessoryDao::getInstance()->selectById(i + 1);
        
        DropItemSprite::DropItemDto dropItemDto4;
        dropItemDto4.objectId = object_count + i + 1;
        dropItemDto4.itemId = mAccessory.getAccessoryId();
        dropItemDto4.itemType = MUseItem::ItemType::EQUIP_ACCESSORY;
        dropItemDto4.imageResId = mAccessory.getAccessoryImageId();
        dropItemDto4.name = mAccessory.getAccessoryName();
        dropItemDto4.isEquip = false;
        
        MapIndex mapIndex4 = tiled_map_layer->getFloorRandomMapIndex(false);
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

MenuItem* RogueScene::createKeypadMenuItemSprite(SpriteFrame* pBaseSpriteFrame, SpriteFrame* pBasePressSpriteFrame, const ccMenuCallback& callback) {
    auto pKeypad = Sprite::createWithSpriteFrame(pBaseSpriteFrame);
    auto pKeypadPress = Sprite::createWithSpriteFrame(pBasePressSpriteFrame);
    
    pKeypadPress->setColor(Color3B::GRAY);
    auto pMenuKey = MenuItemSprite::create(pKeypad, pKeypadPress, callback);
    pMenuKey->setOpacity(128);
    return pMenuKey;
}

Vector<MenuItem*> RogueScene::createKeypadMenuItemArray() {
    Vector<MenuItem*> resultArray;
    
    auto pKeyBase = Sprite::create("ui/keypad.png");
    auto pKeyBasePress = Sprite::create("ui/keypad_press.png");
    
    auto rogue_map_layer = getRogueMapLayer();
    
    auto pMenuKeyUp = createKeypadMenuItemSprite(pKeyBase->getSpriteFrame(), pKeyBasePress->getSpriteFrame(), [this](Ref *pSender) {
        CCLOG("pMenuKeyUpが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
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
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
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
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
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
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
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

Vector<MenuItem*> RogueScene::createButtonMenuItemArray() {
    Size win_size = Director::getInstance()->getWinSize();
    
    Vector<MenuItem*> resultArray;
    
    auto a_button = Sprite::create("ui/a_button.png");
    auto a_buttonPress = Sprite::create("ui/a_button_press.png");
    a_buttonPress->setOpacity(128);
    auto pA_MenuButton = MenuItemSprite::create(a_button, a_buttonPress, [this](Ref* pSender) {
        CCLOG("Aボタンが押された！");
        if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
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
        showItemList();
    });
    pD_MenuButton->setPosition(Point(win_size.width - base_tile_size.width * 2, rogu_map_layer->indexToPoint(11, 2).y));
    pD_MenuButton->setTag(RogueScene::D_ButtonMenuTag);
    resultArray.pushBack(pD_MenuButton);
    
    return resultArray;
}

// floorTitleカットインを生成する
// タッチイベントでフェードアウトしてremoveする
// private
ModalLayer* RogueScene::createFloorTitleCutInLayer(int quest_id) {
    auto win_size = Director::getInstance()->getWinSize();
    const int floor_title_font_size = 47;
    
    auto modal_layer = ModalLayer::create();
    modal_layer->setOpacity(0);
    
    // 真っ黒の全画面で中心にフロア名 N層 と表示され　タップするとフェードアウトして消える
    auto floor_title_cutin_layer = LayerColor::create(Color4B::BLACK);
    floor_title_cutin_layer->setContentSize(win_size);
    // テキスト中央
    // TODO: (kyokomi) タイトルはdaoからとってくる予定
    auto floor_title_text = StringUtils::format("初心者の洞窟 %d層", quest_id);
    auto floor_title_text_label = Label::create(floor_title_text, GAME_FONT(floor_title_font_size), GAME_FONT_SIZE(floor_title_font_size));
    floor_title_text_label->setPosition(Point(floor_title_cutin_layer->getContentSize().width / 2, floor_title_cutin_layer->getContentSize().height / 2));
    floor_title_cutin_layer->addChild(floor_title_text_label);

    auto cutin_listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    cutin_listener->setSwallowTouches(true);
    cutin_listener->onTouchBegan = [floor_title_cutin_layer, modal_layer](Touch* touch, Event* event) -> bool {
        // カットインを破棄
        floor_title_cutin_layer->runAction(Sequence::create(FadeOut::create(1.5f), CallFunc::create([modal_layer]() {
            modal_layer->setVisible(false);
            modal_layer->removeAllChildrenWithCleanup(true);
        }), NULL));
        return false;
    };
    floor_title_cutin_layer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cutin_listener, floor_title_cutin_layer);
    
    modal_layer->addChild(floor_title_cutin_layer);
    
    return modal_layer;
}

// gameoverカットインを生成する
ModalLayer* RogueScene::createGameOverCutInLayer() {
     auto win_size = Director::getInstance()->getWinSize();
    
    auto modal_layer = ModalLayer::create();
    modal_layer->setOpacity(255);
    modal_layer->setColor(Color3B::BLACK);
    
    // ---------------
    
    // TODO: (kyokomi) 画像は一般公開できないので注意
    auto game_over_layer = Sprite::create("game_over.jpg");
    game_over_layer->setPosition(Point(win_size.width / 2, win_size.height / 2));
    
    // ---------------
    modal_layer->addChild(game_over_layer);
    
    // フェードインで登場
    game_over_layer->setOpacity(0);
    game_over_layer->runAction(Sequence::create(FadeIn::create(2.0f), CallFunc::create([this, game_over_layer, modal_layer]() {
        // フェードイン後にタップ可能になる
        auto cutin_listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
        cutin_listener->setSwallowTouches(true);
        cutin_listener->onTouchBegan = [this, game_over_layer, modal_layer](Touch* touch, Event* event) -> bool {
            // カットインを破棄
            game_over_layer->runAction(Sequence::create(FadeOut::create(1.5f), CallFunc::create([this, modal_layer]() {
                modal_layer->setVisible(false);
                modal_layer->removeAllChildrenWithCleanup(true);
                // マイページへ
                this->changeScene(MypageScene::scene());
            }), NULL));
            return false;
        };
        game_over_layer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cutin_listener, game_over_layer);
    }), NULL));
    return modal_layer;
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

void RogueScene::changeGameStatus(GameStatus gameStatus) {
    CCLOG("turn %d change gameStatus %d => %d", rogue_play_data_.turn_count, rogue_play_data_.game_status, gameStatus);
    
    auto rogue_map_layer = getRogueMapLayer();
    auto pPlayer = getPlayerActorSprite(1);
    
    GameStatus beforeGameStatus = rogue_play_data_.game_status;
    rogue_play_data_.game_status = gameStatus;
    
    if (beforeGameStatus == GameStatus::GAME_OVER) {
        return;
    }
    
    if (rogue_play_data_.game_status == GameStatus::GAME_OVER) {
        // ゲームオーバーの演出
        auto game_over_layer = createGameOverCutInLayer();
        this->addChild(game_over_layer, RogueScene::CutInLayerZOrder);
        return;
        
    } else if ((beforeGameStatus == GameStatus::PLAYER_TURN || beforeGameStatus == GameStatus::PLAYER_ACTION || beforeGameStatus == GameStatus::PLAYER_NO_ACTION)
        && rogue_play_data_.game_status == GameStatus::ENEMY_TURN) {
        // 敵のターン開始時
        enemyTurn();
    } else if (rogue_play_data_.game_status == GameStatus::PLAYER_ACTION) {
        rogue_play_data_.no_action_count = 0;
    } else if (rogue_play_data_.game_status == GameStatus::PLAYER_NO_ACTION) {
        rogue_play_data_.no_action_count++;
    } else if (rogue_play_data_.game_status == GameStatus::PLAYER_TURN) {
        // カーソルはクリアする
        MapManager::getInstance()->clearCursor();
        // ターン数を進める
        rogue_play_data_.turn_count++;
        
        // TODO: とりあえずここで・・・
        auto pPlayerDto = pPlayer->getActorDto();
        
        // 10ターンに1空腹度が減るという
        if (rogue_play_data_.turn_count % 10 == 0) {
            if (pPlayerDto->magicPoint > 0)
            {
                pPlayerDto->magicPoint--;
            }
        }
        // 無行動が4ターン続くとHPが回復
        if (rogue_play_data_.no_action_count == 4) {
            rogue_play_data_.no_action_count = 0;
            
            if (pPlayerDto->hitPointLimit > pPlayerDto->hitPoint)
            {
                pPlayerDto->hitPoint++;
            }
        }
        
        // 敵のリポップ
        
        // ランダムなタイミングでランダムに湧く
        int rand = GetRandom(1, 10); // 1%
        if (rand == 1) {
            MapIndex rePopIndex = rogue_map_layer->getFloorRandomMapIndex(true);
            // 敵データ作成
            int enemy_rand_id = GetRandom(1, 8);
            ActorSprite::ActorDto enemyDto = ActorSprite::createActorDto(m_monster::data_.at(StringUtils::format("%d", enemy_rand_id)).asString());
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

void RogueScene::changeScene(Scene* scene) {
    // TODO: とりあえず真っ黒のレイヤーを乗せる
    this->addChild(LayerColor::create(Color4B::BLACK), 99999999, 99999999);
    
    auto trans = TransitionProgressOutIn::create(1, scene);
    Director::getInstance()->replaceScene(trans);
}

float RogueScene::getAnimationSpeed() {
    auto pMenu = getChildByTag(RogueScene::ButtonMenuTag);
    if (pMenu) {
        auto pB_ButtonMenuItem = static_cast<MenuItem*>(pMenu->getChildByTag(RogueScene::B_ButtonMenuTag));
        if (pB_ButtonMenuItem && pB_ButtonMenuItem->isSelected()) {
            return 0.0f;
        }
    }
    
    return 0.2f;
}

#pragma mark
#pragma mark モンスターのAI関連

void RogueScene::enemyTurn() {
    auto rogue_map_layer = getRogueMapLayer();
    
    // モンスターの数だけ繰り返す
    std::list<ActorMapItem> enemyList = MapManager::getInstance()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList) {
        // ランダムでとどまるか移動するかきめる
        int rand = GetRandom(2, 2);
        if (rand == 1) {
            auto pEnemySprite = rogue_map_layer->getEnemyActorSprite(enemyMapItem.seqNo);
            pEnemySprite->getActorMapItem()->moveDone = true;
            
            // とどまる
            logMessage("様子を見ている seqNo = %d", enemyMapItem.seqNo);
        } else if (rand == 2) {
            // プレイヤーに向かって移動 or プレイヤーに攻撃
            auto pPlayerActorSprite = getPlayerActorSprite(1);
            
            // プレイヤーの周辺で最もコストが低い箇所へ移動
            auto playerMapIndex = pPlayerActorSprite->getActorMapItem()->mapIndex;
            std::list<MapIndex> searchMapIndexList = MapManager::createRelatedMapIndexList(playerMapIndex);
            
            // そもそもプレイヤーが隣接しているかチェック
            bool isPlayerAttack = false;
            {
                for (MapIndex mapIndex : searchMapIndexList) {
                    if (MAP_INDEX_DIFF(enemyMapItem.mapIndex, mapIndex)) {
                        isPlayerAttack = true;
                    }
                }
            }
            
            MapIndex moveMapIndex = enemyMapItem.mapIndex;
            if (isPlayerAttack) {
                // 攻撃
                moveMapIndex =pPlayerActorSprite->getActorMapItem()->mapIndex;
                
            } else {
                // 移動可能な経路情報を設定
                MapManager::getInstance()->createActorFindDist(enemyMapItem.mapIndex, enemyMapItem.moveDist);
                
                // 最も移動コストがかからない場所を抽出
                MapItem targetMoveDistMapItem = MapManager::getInstance()->searchTargetMapItem(searchMapIndexList);
                
                // 移動リスト作成
                if (targetMoveDistMapItem.mapDataType == MapDataType::MOVE_DIST) {
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
            
            if (rogue_map_layer->isMapLayerOver(moveMapIndex)) {
                
                CCLOG("移動不可 seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->getActorMapItem()->moveDone = true;
                
            } else if (rogue_map_layer->isTiledMapColisionLayer(moveMapIndex)) {
                
                logMessage("壁ドーン seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->getActorMapItem()->moveDone = true;
                
            } else if (MapManager::getInstance()->getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::ENEMY) {
                
                if (MAP_INDEX_DIFF(enemyMapItem.mapIndex, moveMapIndex)) {
                    //logMessage("待機 seqNo = %d (%d, %d)");
                } else {
                    logMessage("敵ドーン seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                }
                pEnemySprite->getActorMapItem()->moveDone = true;
                
            } else if (MapManager::getInstance()->getActorMapItem(&moveMapIndex)->mapDataType == MapDataType::PLAYER) {
                // 移動中のステータスへ
                changeGameStatus(GameStatus::ENEMY_ACTION);
                
                // 攻撃アニメーション
                float speed = getAnimationSpeed();
                auto pMove1 = MoveTo::create(speed, pEnemySprite->getPosition() + rogue_map_layer->indexToPointNotTileSize(addMoveIndex));
                auto pMove2 = MoveTo::create(speed, pEnemySprite->getPosition());
                
                pEnemySprite->runAction(Sequence::create(pMove1, pMove2,
                                                         CallFunc::create([this, pEnemySprite, pPlayerActorSprite]() {
                    auto player = pPlayerActorSprite->getActorDto();
                    auto enemy = pEnemySprite->getActorDto();
                                        
                    int damage = BattleLogic::exec(*enemy, *player);
                    // 攻撃イベント
                    this->logMessage("%sの攻撃: %sに%dダメージ", enemy->name.c_str(), player->name.c_str(), damage);
                    // オーバーキル判定
                    if (player->hitPoint < damage) {
                        player->hitPoint = 0;
                    } else {
                        player->hitPoint = player->hitPoint - damage;
                    }
                    
                    // プレイヤーステータス更新と死亡判定
                    this->refreshStatus();
                    
                    pEnemySprite->getActorMapItem()->attackDone = true;
                    
                    checkEnmeyTurnEnd();
                    
                }), NULL));
            } else {
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

void RogueScene::checkEnmeyTurnEnd() {
    bool isTurnEnd = true;
    std::list<ActorMapItem> enemyList = MapManager::getInstance()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList) {
        auto pEnemySprite = getRogueMapLayer()->getEnemyActorSprite(enemyMapItem.seqNo);
        auto pEnemyMapItem = pEnemySprite->getActorMapItem();
        if (!pEnemyMapItem->moveDone && !pEnemyMapItem->attackDone) {
            isTurnEnd = false;
            break;
        }
    }
    if (isTurnEnd) {
        changeGameStatus(GameStatus::PLAYER_TURN);
    } else {
        changeGameStatus(GameStatus::ENEMY_TURN);
    }
}

#pragma mark
#pragma mark タッチイベント関連

void RogueScene::touchEventExec(cocos2d::Point touchPoint) {
    
    auto rogue_map_layer = getRogueMapLayer();
    // マップ移動分を考慮
    MapIndex touch_point_map_index = rogue_map_layer->touchPointToIndex(touchPoint - rogue_map_layer->getPosition());
    CCLOG("touchEventExec touchPointMapIndex x = %d y = %d", touch_point_map_index.x, touch_point_map_index.y);
    
    // 画面外判定
    if (rogue_map_layer->isMapLayerOver(touch_point_map_index)) {
        // タッチ判定とみなさない
        CCLOG("画面外");
        return;
    }
    
    // タッチした位置が有効なIndexか判定
    auto actor_map_item = getPlayerActorSprite(1)->getActorMapItem();
    MapIndex add_move_index = MapManager::checkTouchEventIndex(actor_map_item->mapIndex, touch_point_map_index);
    if (add_move_index.x == 0 && add_move_index.y == 0) {
        // タッチ判定とみなさない
        CCLOG("有効じゃない");
        return;
    }
    touchEventExec(add_move_index, touch_point_map_index);
}

void RogueScene::touchEventExec(MapIndex addMoveIndex, MapIndex touchPointMapIndex) {
    auto rogue_map_layer = getRogueMapLayer();
    
    // キャラの向きを変更
    auto actor_sprite = getPlayerActorSprite(1);
    actor_sprite->runMoveAction(addMoveIndex);
    actor_sprite->getActorMapItem()->mapIndex.moveDictType = addMoveIndex.moveDictType;
    
    // 敵をタッチした
    auto enemy_map_item = MapManager::getInstance()->getActorMapItem(&touchPointMapIndex);
    if (enemy_map_item->mapDataType == MapDataType::ENEMY) {
        // 向きだけ変えてターン経過しない
    } else {
        // 障害物判定
        if (rogue_map_layer->isTiledMapColisionLayer(touchPointMapIndex)) {
            // TODO: ぶつかるSE再生
            logMessage("壁ドーン SE再生");
            
            // ターン経過なし
            
        } else {
            
            changeGameStatus(GameStatus::PLAYER_NO_ACTION);
            
            // アイテムに重なったときの拾う処理
            auto touch_point_map_item = MapManager::getInstance()->getMapItem(&touchPointMapIndex);
            if (touch_point_map_item->mapDataType == MapDataType::MAP_ITEM) {

                // itemを取得
                auto drop_map_item = static_cast<DropMapItem*>(touch_point_map_item);
                auto drop_item_sprite = rogue_map_layer->getDropItemSprite(drop_map_item->seqNo);
                auto drop_item_dto = drop_item_sprite->getDropItemDto();
                
                // ゴールドは別扱い
                if (drop_item_dto->itemType == MUseItem::ItemType::GOLD) {

                    // ゴールド値を乱数で決める
                    // TODO: 乱数の範囲はフロア情報で管理する
                    int gold = GetRandom(10, 999);
                    
                    // メッセージログ
                    logMessage("%d%sを拾った。", gold, drop_item_dto->name.c_str());
                    // ゴールドを加算
                    actor_sprite->getActorDto()->gold += gold;
                    
                    // Map上から削除する
                    rogue_map_layer->removeDropItemSprite(drop_item_sprite);
                    
                } else {
                    // ゴールド以外はインベントリへ
                    
                    if (getItemWindowLayer()->getItemList().size() < USE_ITEM_MAX) {
                        // ドロップアイテムを拾う
                        
                        // TODO: 拾うSE再生
                        
                        // メッセージログ
                        logMessage("%sを拾った。", drop_item_dto->name.c_str());
                        
                        // イベントリに追加する
                        getItemWindowLayer()->addItemList(*drop_item_dto);
                        
                        // Map上から削除する
                        rogue_map_layer->removeDropItemSprite(drop_item_sprite);
                    } else {
                        // アイテム所持数限界
                        logMessage("持ち物が一杯で、\n%sを拾えなかった。", drop_item_dto->name.c_str());
                    }
                }
                
            } else if (touch_point_map_item->mapDataType == MapDataType::KAIDAN) {
                // TODO: (kyokomi)階段SE
                
                // 階段下りる判定
                
                this->showCommonWindow("階段です。\n　\n次の階に進みますか？", [this, actor_sprite](Ref* pSender) {
                    // OK
                    
                    // 閉じる
                    this->hideCommonWindow();
                    
                    // save
                    rogue_play_data_.quest_id += 1;
                    
                    AccountData::getInstance()->player_actor_ = *(actor_sprite->getActorDto());
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
            rogue_map_layer->movePlayerMap(actor_sprite, addMoveIndex, getAnimationSpeed(), CallFunc::create([this](void) {
                // ターンエンド
                changeGameStatus(GameStatus::ENEMY_TURN);
            }));
            
            // コールバックまでgameStatusを更新はしない
        }
    }
}

void RogueScene::attack() {
    auto rogue_map_layer = getRogueMapLayer();
    auto pActorSprite = getPlayerActorSprite(1);
    
    MapIndex addMapIndex = {0, 0, MoveDirectionType::MOVE_NONE};
    MapIndex attackMapIndex = pActorSprite->getActorMapItem()->mapIndex;
    if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_UP) {
        // 上
        addMapIndex.y += 1;
        attackMapIndex.y += 1;
    } else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_RIGHT) {
        // 右
        addMapIndex.x += 1;
        attackMapIndex.x += 1;
    } else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_LEFT) {
        // 左
        addMapIndex.x -= 1;
        attackMapIndex.x -= 1;
    } else if (attackMapIndex.moveDictType == MoveDirectionType::MOVE_DOWN) {
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
    if (pEnemyMapItem->mapDataType == MapDataType::ENEMY) {
        pEnemySprite = rogue_map_layer->getEnemyActorSprite(pEnemyMapItem->seqNo);
    } else {
        // 空振り
    }
    
    // アニメーション
    pActorSprite->runAction(Sequence::create(pMove1, pMove2, CallFunc::create([this, pActorSprite, pEnemySprite](void) {
        if (pEnemySprite) {
            auto player = pActorSprite->getActorDto();
            auto enemy = pEnemySprite->getActorDto();
            
            // 攻撃開始
            int damage = BattleLogic::exec(*player, *enemy);
            // 攻撃イベント
            logMessage("%sの攻撃: %sに%dのダメージ", player->name.c_str(), enemy->name.c_str(), damage);
            // オーバーキル考慮しない
            enemy->hitPoint = enemy->hitPoint - damage;
            
            // 敵の死亡判定
            if (enemy->hitPoint <= 0) {
                logMessage("%sを倒した。経験値%dを得た。", enemy->name.c_str(), enemy->exp);
                
                // TODO: (kyokomi) 経験値更新（計算式 適当）
                player->exp += enemy->exp;
                if (MLevelDao::getInstance()->checkLevelUp(player->lv, player->exp)) {
                    player->lv++;
                    auto mLevel = MLevelDao::getInstance()->selectById(player->lv);
                    player->hitPointLimit += mLevel.getGrowHitPoint();
                    
                    // TODO: レベルアップ演出（SE？）
                    
                    logMessage("%sはレベル%dになった。", player->name.c_str(), player->lv);
                    
                    // レベル上がってステータスが上がるかもしれないので攻撃力、防御力のステータスを更新する
                    this->refreshStatusEquip(*player);
                }
                
                // マップから消える
                getRogueMapLayer()->removeEnemyActorSprite(pEnemySprite);
            }
        }
        changeGameStatus(GameStatus::ENEMY_TURN);
        
    }), NULL));
}

#pragma mark
#pragma mark UI関連

void RogueScene::logMessage(const char * pszFormat, ...) {
    va_list ap;
    va_start(ap, pszFormat);
    char buf[RogueScene::MAX_LOG_LENGTH];
    vsnprintf(buf, RogueScene::MAX_LOG_LENGTH-3, pszFormat, ap);
    va_end(ap);
    
    CCLOG("logMessage: %s", buf);
    
    auto pGameLogNode = getChildByTag(RogueScene::GameLogLayerTag);
    // とりあえず子要素がないなら無理
    if (!pGameLogNode || pGameLogNode->getChildrenCount() <= 0) {
        return;
    }
    
    // TODO: 1子しかaddしてないから動く。ちゃんとしないと・・・
    auto pGameLogText = static_cast<Label*>(pGameLogNode->getChildren().at(1));
    if (pGameLogText) {
        // TODO: 別クラスにしてログをlistで保持する。デフォルトの表示は1件だけで、center寄せ表示でいいかと
        auto pMessage = String::create(buf);
        
        pMessage->append("\n");

        std::string nowString = pGameLogText->getString();
        
        long count = f_r(nowString, '\n');
        
        // 3行まで表示
        if (count >= 3) {
            long size = nowString.size();
            unsigned long loc = nowString.find_last_of('\n', size);
            CCLOG("logMessage: loc = %ld size = %ld", loc, size);
            if (loc != std::string::npos) {
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

void RogueScene::showItemList() {
    
    auto win_size = Director::getInstance()->getWinSize();
    
    auto item_list_modal_layer = getModalLayer();
    item_list_modal_layer->setVisible(true);
    
    auto item_window_layer = getItemWindowLayer();
    if (item_window_layer) {
        item_window_layer->reloadItemList();
        item_window_layer->setVisible(true);
    } else {
        // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
        item_window_layer = ItemWindowLayer::createWithContentSize(win_size * 0.8);
        item_window_layer->setPosition(Point(win_size.width / 2 - item_window_layer->getContentSize().width / 2,
                                            win_size.height /2 - item_window_layer->getContentSize().height / 2));
        item_window_layer->setItemDropMenuCallback([this](Ref* ref, DropItemSprite::DropItemDto drop_item) {
            CCLOG("RogueScene::itemDropMenuCallback");
            
            auto player_sprite = getPlayerActorSprite(1);
            
            // すでにアイテムが置いてある場合は置けない
            if (MapManager::getInstance()->getDropMapItem(&player_sprite->getActorMapItem()->mapIndex)->mapDataType != MapDataType::NONE) {
                this->logMessage("%sを床におけなかった。", drop_item.name.c_str());
                // アイテムを戻す
                this->getItemWindowLayer()->addItemList(drop_item);
                
            } else {
                
                // 装備してたら外す
                if (drop_item.isEquip) {
                    drop_item.isEquip = false;
                    
                    if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
                        player_sprite->equipReleaseWeapon();
                    } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                        player_sprite->equipReleaseAccessory();
                    }
                    
                    // 装備変更でステータス更新
                    refreshStatusEquip(*player_sprite->getActorDto());
                }
                
                // アイテムをマップのプ
                // レイヤーの足元に置く
                if (this->getRogueMapLayer()->tileSetDropMapItem(drop_item, player_sprite->getActorMapItem()->mapIndex)) {
                    this->logMessage("%sを床においた。", drop_item.name.c_str());
                    
                    // ターン消費
                    this->changeGameStatus(RogueScene::ENEMY_TURN);
                }
            }
            
            // インベントリは閉じる
            this->hideItemList();
        });
        
        item_window_layer->setItemUseMenuCallback([this](Ref* ref, DropItemSprite::DropItemDto drop_item) {
            CCLOG("RogueScene::itemUseMenuCallback");
            
            auto player_sprite = getPlayerActorSprite(1);
            
            // itemIdで処理してくれるlogicへ
            std::string use_message = ItemLogic::use(drop_item.itemId, player_sprite->getActorDto());
            
            this->logMessage(use_message.c_str());
            
            // インベントリは閉じる
            this->hideItemList();
            
            // ターン消費
            this->changeGameStatus(RogueScene::ENEMY_TURN);
        });
        
        item_window_layer->setItemEquipMenuCallback([this, item_window_layer](Ref* ref, DropItemSprite::DropItemDto drop_item) {
            CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", drop_item.itemType);
            
            auto player_sprite = getPlayerActorSprite(1);
            
            if (drop_item.isEquip) {
                if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
                    // 解除
                    item_window_layer->setItemEquip(player_sprite->getActorDto()->equip.weaponObjectId, false);

                    // 武器装備
                    MWeapon weapon_master = MWeaponDao::getInstance()->selectById(drop_item.itemId);
                    player_sprite->getActorDto()->equip.weaponObjectId = drop_item.objectId;
                    player_sprite->getActorDto()->equip.weaponId       = weapon_master.getWeaponId();
                    player_sprite->getActorDto()->equip.weaponImgResId = weapon_master.getWeaponImageId();
                    player_sprite->getActorDto()->equip.weaponName     = weapon_master.getWeaponName();
                    player_sprite->getActorDto()->equip.weaponStr      = weapon_master.getAttackPoint();
                    
                } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    // 解除
                    item_window_layer->setItemEquip(player_sprite->getActorDto()->equip.accessoryObjectId, false);
                    // 防具装備
                    MAccessory accessory_master = MAccessoryDao::getInstance()->selectById(drop_item.itemId);
                    player_sprite->getActorDto()->equip.accessoryObjectId = drop_item.objectId;
                    player_sprite->getActorDto()->equip.accessoryId       = accessory_master.getAccessoryId();
                    player_sprite->getActorDto()->equip.accessoryImgResId = accessory_master.getAccessoryImageId();
                    player_sprite->getActorDto()->equip.accessoryName     = accessory_master.getAccessoryName();
                    player_sprite->getActorDto()->equip.accessoryDef      = accessory_master.getDefensePoint();
                }
                this->logMessage("%sを装備した。", drop_item.name.c_str());
            } else {
                if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
                    // 武器解除
                    player_sprite->equipReleaseWeapon();
                } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    // 防具装備
                    player_sprite->equipReleaseAccessory();
                }
                this->logMessage("%sの装備をはずした。", drop_item.name.c_str());
            }
            
            // 装備解除、装備によってステータス変動するためステータスバーを更新
            this->refreshStatusEquip(*player_sprite->getActorDto());
            
            // インベントリは閉じる
            this->hideItemList();
            
            // ターン消費
            this->changeGameStatus(RogueScene::ENEMY_TURN);
        });
        
        item_list_modal_layer->addChild(item_window_layer, RogueScene::ItemListLayerZOrder, RogueScene::ItemListWindowTag);
        
        // ---- button -----
        
        const int font_size = 20;
        
        // 並び替えボタン
        auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::create("並び替え", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), Size(12, 4), [this, item_window_layer](Ref* ref) {
            // 並び替え
            item_window_layer->sortItemList();
            item_window_layer->reloadItemList();
        });
        sort_menu_item_label->setPosition(Point(item_window_layer->getContentSize().width, item_window_layer->getContentSize().height + sort_menu_item_label->getContentSize().height / 2));
        
        // 閉じるボタン
        auto clone_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::create("とじる", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), Size(12, 4), [this](Ref* ref) {
            // 閉じる
            this->hideItemList();
        });
        clone_menu_item_label->setPosition(Point(item_window_layer->getContentSize().width, clone_menu_item_label->getPositionY() -  clone_menu_item_label->getContentSize().height / 2));
        
        auto menu = Menu::create(sort_menu_item_label, clone_menu_item_label, NULL);
        menu->setPosition(Point::ZERO);
        
        item_window_layer->addChild(menu);
    }
}

void RogueScene::hideItemList() {
    // モーダルレイヤー非表示にする
    auto item_window_layer = getItemWindowLayer();
    if (item_window_layer) {
        item_window_layer->setVisible(false);
    }
    
    auto modal_layer = getModalLayer();
    if (modal_layer) {
        modal_layer->setVisible(false);
    }
}

void RogueScene::showCommonWindow(std::string titleText, const ccMenuCallback& okMenuItemCallback, const ccMenuCallback& ngMenuItemCallback) {
    auto winSize = Director::getInstance()->getWinSize();
    
    // モーダルレイヤー作成
    auto pModalLayer = getModalLayer();
    pModalLayer->setVisible(true);
    
    auto pCommonWindow = getCommonWindow();
    if (pCommonWindow) {
        pCommonWindow->setVisible(true);
    } else {
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

AlertDialogLayer* RogueScene::getCommonWindow() {
    auto pModalLayer = getModalLayer();
    return static_cast<AlertDialogLayer*>(pModalLayer->getChildByTag(RogueScene::CommonWindowTag));
}

void RogueScene::hideCommonWindow() {
    auto pCommonWindow = getCommonWindow();
    if (pCommonWindow) {
        pCommonWindow->setVisible(false);
    }
    
    auto pModalLayer = getModalLayer();
    if (pModalLayer) {
        pModalLayer->setVisible(false);
    }
}

void RogueScene::refreshStatus() {
    
    auto pStatusBarLayer = getChildByTag(RogueScene::StatusBarLayerTag);
    if (!pStatusBarLayer) {
        return;
    }
    
    auto pStatusText = pStatusBarLayer->getChildren().at(0); // TODO: とりあえず1要素なので。。。
    if (pStatusText) {
        
        // プレイヤー取得
        auto pPlayerSprite = getPlayerActorSprite(1);
        auto pPlayerDto = pPlayerSprite->getActorDto();
        int floor = rogue_play_data_.quest_id; // フロア情報（クエストID=フロア数でいい？)
        // 作成
        auto pStr = String::createWithFormat(" %2dF Lv%3d HP %3d/%3d 満腹度 %d/%d %10d G", floor, pPlayerDto->lv, pPlayerDto->hitPoint, pPlayerDto->hitPointLimit, pPlayerDto->magicPoint, pPlayerDto->magicPointLimit, pPlayerDto->gold);
        
        auto pLabelText = static_cast<Label*>(pStatusText);
        pLabelText->setString(pStr->getCString());
        pLabelText->setPositionX(pLabelText->getContentSize().width / 2);
        
        // TODO: 死亡判定ここで？
        if (pPlayerDto->hitPoint == 0) {
            logMessage("%sは死亡した。", pPlayerDto->name.c_str());
            changeGameStatus(RogueScene::GameStatus::GAME_OVER);
        } else if (pPlayerDto->magicPoint == 0) {
            logMessage("%sは空腹で倒れた。", pPlayerDto->name.c_str());
            changeGameStatus(RogueScene::GameStatus::GAME_OVER);
        } else {
            // 残りHPで文字色を変える
            float hitPointDiv = (float)pPlayerDto->hitPoint / (float)pPlayerDto->hitPointLimit;
            float mpDiv = (float)pPlayerDto->magicPoint / (float)pPlayerDto->magicPointLimit;
            if (hitPointDiv <= 0.25 || mpDiv <= 0.10) {
                pLabelText->setColor(Color3B::RED);
            } else if (hitPointDiv <= 0.50 || mpDiv <= 0.30) {
                pLabelText->setColor(Color3B::YELLOW);
            } else {
                pLabelText->setColor(Color3B::WHITE);
            }
        }
    }
}

// 装備状態更新
void RogueScene::refreshStatusEquip(const ActorSprite::ActorDto& actorDto) {
    
    auto statusLayer = dynamic_cast<Layer*>(getChildByTag(RogueScene::StatusBarLayerTag));
    if (statusLayer) {
        // 武器
        auto equipWeaponLayer = dynamic_cast<Layer*>(statusLayer->getChildByTag(RogueScene::StatusBarEquipWeaponTag));
        if (equipWeaponLayer) {
            // TODO: (kyokomi) とりあえず...サーセン
            const int SpriteTag = 1;
            auto sprite = dynamic_cast<Sprite*>(equipWeaponLayer->getChildByTag(SpriteTag));
            // TODO: (kyokomi) とりあえず...サーセン
            const int LabelTag = 2;
            auto label = dynamic_cast<Label*>(equipWeaponLayer->getChildByTag(LabelTag));
            if (actorDto.equip.weaponId > 0) {
                std::string spriteFileName = DropItemSprite::createItemImageFileName(actorDto.equip.weaponImgResId);
                auto equipSpriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFileName);
                if (!equipSpriteFrame) {
                    equipSpriteFrame = cocos2d::Sprite::create(spriteFileName)->getSpriteFrame();
                    cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(equipSpriteFrame, spriteFileName);
                }
                sprite->setVisible(true);
                sprite->setSpriteFrame(equipSpriteFrame);
                std::string labelText = StringUtils::format("%3d", actorDto.attackPoint + actorDto.equip.weaponStr);
                label->setString(labelText);
            } else {
                sprite->setVisible(false);
                
                std::string labelText = StringUtils::format("%3d", actorDto.attackPoint);
                label->setString(labelText);
            }
        }
        
        // 防具
        auto equipAccessoryLayer = dynamic_cast<Layer*>(statusLayer->getChildByTag(RogueScene::StatusBarEquipAccessoryTag));
        if (equipAccessoryLayer) {
            // TODO: (kyokomi) とりあえず...サーセン
            const int SpriteTag = 1;
            auto sprite = dynamic_cast<Sprite*>(equipAccessoryLayer->getChildByTag(SpriteTag));
            // TODO: (kyokomi) とりあえず...サーセン
            const int LabelTag = 2;
            auto label = dynamic_cast<Label*>(equipAccessoryLayer->getChildByTag(LabelTag));
        
            if (actorDto.equip.accessoryId > 0) {
                std::string spriteFileName = DropItemSprite::createItemImageFileName(actorDto.equip.accessoryImgResId);
                auto equipSpriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFileName);
                if (!equipSpriteFrame) {
                    equipSpriteFrame = cocos2d::Sprite::create(spriteFileName)->getSpriteFrame();
                    cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(equipSpriteFrame, spriteFileName);
                }
                sprite->setVisible(true);
                sprite->setSpriteFrame(equipSpriteFrame);
                
                std::string labelText = StringUtils::format("%3d", actorDto.defencePoint + actorDto.equip.accessoryDef);
                label->setString(labelText);
            } else {
                sprite->setVisible(false);
                
                std::string labelText = StringUtils::format("%3d", actorDto.defencePoint);
                label->setString(labelText);
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

ActorSprite* RogueScene::getPlayerActorSprite(int seqNo) {
    return static_cast<ActorSprite*>(getChildByTag(RogueScene::ActorPlayerTag + seqNo));
}

ItemWindowLayer* RogueScene::getItemWindowLayer() {
    auto pModalLayer = getModalLayer();
    return static_cast<ItemWindowLayer*>(pModalLayer->getChildByTag(RogueScene::ItemListWindowTag));
}

// TODO: (kyokomi)これイマイチ。ItemWindowとCommonWindowで共用しちゃってる
ModalLayer* RogueScene::getModalLayer() {
    // モーダルレイヤー作成
    auto pModalLayer = static_cast<ModalLayer*>(this->getChildByTag(RogueScene::ModalLayerTag));
    if (pModalLayer) {
        //pModalLayer->setVisible(true);
    } else {
        pModalLayer = ModalLayer::create();
        this->addChild(pModalLayer, RogueScene::ModalLayerZOrder, RogueScene::ModalLayerTag);
    }
    
    return pModalLayer;
}

RogueTMXTiledMap* RogueScene::getRogueMapLayer() {
    return static_cast<RogueTMXTiledMap*>(this->getChildByTag(RogueScene::TiledMapLayerTag));
}


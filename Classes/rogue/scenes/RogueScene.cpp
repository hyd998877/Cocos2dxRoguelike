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
#include "LayerActionUtils.h"
#include "LotteryUtils.h"
#include "StringUtils.h"

#include "SystemMenuLayer.h"
#include "ItemInventoryLayer.h"
#include "AlertDialogLayer.h"
#include "ModalLayer.h"

#include "ItemLogic.h"
#include "BattleLogic.h"

#include "MLevelDao.h"
#include "MWeaponDao.h"
#include "MUseItemDao.h"
#include "MAccessoryDao.h"
#include "MMonster.h"
#include "MPlayer.h"
#include "MRogueMap.h"

#include "AccountData.h"

#include "TopScene.h"
#include "NovelScene.h"

#include "KeypadLayout.h"
#include "HeaderStatusLayout.h"
#include "GameLogLayout.h"

#include "WidgetUtil.h"

NS_ROGUE_BEGIN

#pragma mark
#pragma mark main

RogueScene::RogueScene()
: _beforeGameStatus(RoguePlayDto::GameStatus::INIT)
, _gameStatus(RoguePlayDto::GameStatus::INIT)
, _roguePlayDto()
, _itemInventory()
, _keypadLayout(nullptr)
, _statusWidget(nullptr)
{
    CCLOG("new rogueScene");
}

RogueScene::~RogueScene()
{
    CCLOG("death rogueScene");
}

Scene* RogueScene::scene(RoguePlayDto::QuestType questType, int questId) {
    Scene *scene = Scene::create();
    RogueScene *layer = RogueScene::createWithQuestId(questType, questId);
    scene->addChild(layer);
    return scene;
}

RogueScene* RogueScene::createWithQuestId(RoguePlayDto::QuestType questType, int questId) {
    auto *pRet = new RogueScene();
    if (pRet && pRet->initWithQuestId(questType, questId)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool RogueScene::initWithQuestId(RoguePlayDto::QuestType questType, int quest_id) {
    
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    // 乱数初期化
    srand((unsigned int)time(NULL));

    // データロード
    AccountData::getInstance()->load();
    
    ActorDto actor_dto;
    // 不一致の場合初期化
    if (!AccountData::getInstance()->isPlayQuestData(questType, quest_id)) {
        AccountData::getInstance()->resetRoguePlayData();
        this->_roguePlayDto.setPlayQuest(questType, quest_id);
        // デフォルトステータス
        actor_dto = ActorDto::createActorDto(m_player::data_.at("1").asString());
    } else {
        // ロード処理
        this->_roguePlayDto = AccountData::getInstance()->getRoguePlayData();
        actor_dto = AccountData::getInstance()->getPlayerActor();
    }
    // イベントリロード
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    
    auto win_size = Director::getInstance()->getWinSize();
    
    // ---------------------
    // フロア開始カットイン表示
    // ---------------------
    playFloorTitleCutIn(this->_roguePlayDto.getQuestId(), this->_roguePlayDto.getQuestType());
    
    // ---------------------
    // タイルマップを生成
    // ---------------------
    // TODO: (kyokomi)ランダムなマップIDを指定する
    this->_roguePlayDto.setFloorId(2);
    
    auto tmxFileName = cocos2d::StringUtils::format("tmx/quest_%d.tmx", this->_roguePlayDto.getFloorId());
    auto tiled_map_layer = RogueTMXTiledMap::create(tmxFileName);

    tiled_map_layer->setPosition(Point::ZERO);
    this->addChild(tiled_map_layer, ZOrders::TiledMapLayerZOrder, Tags::TiledMapLayerTag);
    
    //-------------------------
    // ステータスバー
    //-------------------------
    this->_statusWidget = HeaderStatusLayout::create();
    this->addChild(this->_statusWidget, ZOrders::StatusBarLayerZOrder);
    
    //-------------------------
    // ゲームログ表示
    //-------------------------
    this->_gameLogWidget = GameLogLayout::create();
    this->addChild(this->_gameLogWidget, ZOrders::GameLogLayerZOrder, Tags::GameLogLayerTag);
    
    // ------------------------
    // ミニマップ
    // ------------------------
    auto mini_map_layer = tiled_map_layer->getMiniMapLayer();
    // ステータスバーの下くらい
    mini_map_layer->setPosition(0, mini_map_layer->getPositionY() + win_size.height - mini_map_layer->getContentSize().height - (win_size.height * 0.1));
    this->addChild(mini_map_layer, ZOrders::MiniMapLayerZOrder, Tags::MiniMapLayerTag);
    
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
    this->addChild(actor_sprite, ZOrders::ActorPlayerZOrder, (Tags::ActorPlayerTag + actor_map_item.seqNo));
    // マップに追加
    tiled_map_layer->setPlayerActorMapItem(actor_sprite->getActorMapItem(), actor_sprite->getTag());
    
    refreshStatusEquip(actor_dto);
    // ---------------------
    // 敵キャラ生成
    // ---------------------
    int probCount = getRogueMapData().at(RogueGameConfig::MobCount).asInt();
    institutionEnemy(probCount);
    
    //-------------------------
    // アイテム配置
    //-------------------------
    int dropItemCount = getRogueMapData().at(RogueGameConfig::DropItemCount).asInt();
    institutionDropItem(dropItemCount);
    
    // -------------------------------
    // メニュー
    // -------------------------------
    setupKeypadLayout();
    
    // ---------------------------------
    // プレイヤーの先行
    changeGameStatus(RoguePlayDto::GameStatus::PLAYER_TURN);
    
    //----------------------------------
    // 照明
    rogueMapLighting();
    
    return true;
}

void RogueScene::setupKeypadLayout()
{
    auto keypad = KeypadLayout::create();
    keypad->setKeypadCallback(KeypadLayout::ButtonType::UP, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        auto mapIndex = getNowRogueMapIndex();
        this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x, mapIndex.y + 1));
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::LEFT, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        auto mapIndex = getNowRogueMapIndex();
        this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x - 1, mapIndex.y));
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::RIGHT, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        auto mapIndex = getNowRogueMapIndex();
        this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x + 1, mapIndex.y));
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::DOWN, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        auto mapIndex = getNowRogueMapIndex();
        this->touchEventExec(getRogueMapLayer()->indexToPoint(mapIndex.x, mapIndex.y - 1));
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::A, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        this->attack();
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::B, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::C, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        this->showSystemMenu();
    });
    keypad->setKeypadCallback(KeypadLayout::ButtonType::D, [this]() {
        if (!this->isKeypadControll()) {
            return;
        }
        this->showItemInventoryWindow();
    });
    this->addChild(keypad, ZOrders::MenuLayerZOrder);
    _keypadLayout = keypad;
}

MapIndex RogueScene::getNowRogueMapIndex()
{
    auto winSize = Director::getInstance()->getWinSize();
    Point point = Point(winSize.width / 2, winSize.height / 2);
    return getRogueMapLayer()->pointToIndex(point);
}

bool RogueScene::isKeypadControll()
{
    if (this->_gameStatus == RoguePlayDto::GameStatus::PLAYER_TURN) {
        return true;
    }
    
    return false;
}


void RogueScene::showItemInventoryWindow()
{
    // メニュー消す
    _keypadLayout->hideKeypadMenu();
    
    auto itemWindowLayer = ItemInventoryLayer::create(this->_itemInventory);

    // 足元にアイテムないがない場合で「捨てる/交換」を切り替える
    ItemInventoryLayer::ActionCallback itemDropMenuCallback;
    {
        auto player_sprite = getPlayerActorSprite(1);
        MapIndex targetMapIndex = player_sprite->getActorMapItem().mapIndex;
        
        if (this->getMapManager()->isDropMapItemIndex(targetMapIndex)) {
            itemDropMenuCallback = ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_DROP, ItemInventoryLayer::CloseType::CLOSE,
                    [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                    CCLOG("RogueScene::itemDropMenuCallback");
                    this->itemWindowDropItem(itemDto);
                }
            };
        } else {
            itemDropMenuCallback = ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_CHANGE, ItemInventoryLayer::CloseType::CLOSE,
                [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                    CCLOG("RogueScene::itemChangeMenuCallback");
                    this->itemWindowChangeItem(itemDto);
                }
            };
        }
    }
    std::list<ItemInventoryLayer::ActionCallback> itemMenuList {
        itemDropMenuCallback,
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_USE, ItemInventoryLayer::CloseType::CLOSE,
            [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                CCLOG("RogueScene::itemUseMenuCallback");
                this->itemWindowUseItem(itemDto);
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_EQUIP, ItemInventoryLayer::CloseType::CLOSE,
            [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", itemDto.getItemType());
                this->itemWindowEquipItem(itemDto);
            }
        },
    };
    itemWindowLayer->initMenuActionCallback(itemMenuList);
    itemWindowLayer->setSortCallback([this](ItemInventoryDto::Comparator comparator) {
        this->_itemInventory.sortItemList(comparator);
    });
    itemWindowLayer->setCloseCallback([this]() {
        // メニュー戻す
        _keypadLayout->showKeypadMenu();
    });
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer, this));
    
    auto novelActorSprite = ActorSprite::createNovelActorSprite(ActorSprite::NovelDirection::RIGHT);
    itemWindowLayer->addChild(novelActorSprite, -1);
    
    this->addChild(itemWindowLayer, ZOrders::ItemListLayerZOrder, Tags::ItemListWindowTag);
}

void RogueScene::itemWindowDropItem(const ItemDto &itemDto)
{
    // アイテムをマップのプレイヤーの足元に置く。ただしすでにアイテムが置いてある場合は置けない
    auto player_sprite = getPlayerActorSprite(1);
    MapIndex targetMapIndex = player_sprite->getActorMapItem().mapIndex;
    if (!this->getRogueMapLayer()->tileSetDropMapItem(itemDto, targetMapIndex)) {
        this->logMessage(itemDto.createItemName() + "を床におけなかった。");
        return;
    }
    
    std::string message = itemDto.createItemName() + "を床においた。";
    // もし装備してたら外す
    unEquipItem(itemDto);
    this->_itemInventory.removeItemDto(itemDto.getObjectId());
    
    // ターン消費
    this->changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
    
    this->logMessage(message);
}

void RogueScene::unEquipItem(const ItemDto &itemDto)
{
    auto playerSprite = getPlayerActorSprite(1);
    // もし装備してたら外す
    if (itemDto.isEquip()) {
        this->_itemInventory.itemEquip(itemDto.getObjectId(), false);
        if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON) {
            playerSprite->getActorDto()->equipReleaseWeapon();
        } else if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
            playerSprite->getActorDto()->equipReleaseAccessory();
        }
        
        // 装備変更でステータス更新
        this->refreshStatusEquip(*playerSprite->getActorDto());
    }
}
void RogueScene::itemWindowChangeItem(const ItemDto &itemDto)
{
    auto player_sprite = getPlayerActorSprite(1);
    MapIndex targetMapIndex = player_sprite->getActorMapItem().mapIndex;
    
    if (this->getMapManager()->isDropMapItemIndex(targetMapIndex)) {
        this->logMessage("交換できなかった");
        return;
    }
    
    // 交換対象
    auto targetMapItem = this->getMapManager()->getDropMapItem(targetMapIndex);
    // 拾う
    auto touchItemDto = touchDropItem(targetMapItem, false);

    // もし装備してたら外す
    unEquipItem(itemDto);
    this->getRogueMapLayer()->tileSetDropMapItem(itemDto, targetMapIndex);
    this->_itemInventory.removeItemDto(itemDto.getObjectId());
    
    // ターン消費
    this->changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
    
    this->logMessage(itemDto.createItemName() + "を床において\n" +
                     touchItemDto.createItemName() + "を拾った。");
}

void RogueScene::itemWindowEquipItem(const ItemDto &itemDto)
{
    auto player_sprite = this->getPlayerActorSprite(1);
    
    if (!itemDto.isEquip()) {
        long relaseObjectId = 0L;
        
        if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON) {
            // 解除
            relaseObjectId = player_sprite->getActorDto()->getWeaponEquip().getObjectId();
            // 武器装備
            MWeapon mWeapon = MWeaponDao::getInstance()->selectById(itemDto.getItemId());
            player_sprite->getActorDto()->equipWeapon(itemDto.getObjectId(), itemDto.getParam(), mWeapon);
            this->_itemInventory.itemEquip(itemDto.getObjectId(), true);
            
        } else if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
            // 解除
            relaseObjectId = player_sprite->getActorDto()->getAccessoryEquip().getObjectId();
            // 防具装備
            MAccessory mAccessory = MAccessoryDao::getInstance()->selectById(itemDto.getItemId());
            player_sprite->getActorDto()->equipAccessory(itemDto.getObjectId(), itemDto.getParam(), mAccessory);
        }
        
        if (relaseObjectId != 0) {
            this->_itemInventory.itemEquip(relaseObjectId, false);
        }
        this->_itemInventory.itemEquip(itemDto.getObjectId(), true);
        auto message = cocos2d::StringUtils::format("%sを装備した。", itemDto.createItemName().c_str());
        this->logMessage(message);
    } else {
        if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON) {
            // 武器解除
            player_sprite->getActorDto()->equipReleaseWeapon();
        } else if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
            // 防具解除
            player_sprite->getActorDto()->equipReleaseAccessory();
        }
        this->_itemInventory.itemEquip(itemDto.getObjectId(), false);
        auto message = cocos2d::StringUtils::format("%sの装備をはずした。", itemDto.createItemName().c_str());
        this->logMessage(message);
    }
    
    // 装備解除、装備によってステータス変動するためステータスバーを更新
    this->refreshStatusEquip(*player_sprite->getActorDto());
    
    // ターン消費
    this->changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
}

void RogueScene::itemWindowUseItem(const ItemDto &itemDto)
{
    auto player_sprite = this->getPlayerActorSprite(1);
    
    // itemIdで処理してくれるlogicへ
    std::string use_message = ItemLogic::use(itemDto.getItemId(), player_sprite->getActorDto());
    
    this->logMessage(use_message);
    
    this->_itemInventory.removeItemDto(itemDto.getObjectId());
    
    // ターン消費
    this->changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
}

// --------------------------
// TODO: CutInクラス作ってcreateしてattachしてrunする感じがいい気がする
// --------------------------
// floorTitleカットインを生成する
// タッチイベントでフェードアウトしてremoveする
// private
void RogueScene::playFloorTitleCutIn(int quest_id, RoguePlayDto::QuestType questType)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    auto modalLayer = ModalLayer::create(Color3B::BLACK, 0);
    
    // 真っ黒の全画面で中心にフロア名 N層 と表示され　タップするとフェードアウトして消える
    auto floorTitleCutInLayer = LayerColor::create(Color4B::BLACK);
    floorTitleCutInLayer->setContentSize(winSize);
    // テキスト中央
    auto questKey = RoguePlayDto::findQuestKey(questType);
    auto questName = RogueGameConfig::getQuestData(questKey).at("name").asString();
    
    auto floorTitleText = cocos2d::StringUtils::format("%s %d層", questName.c_str(), quest_id);
    auto floorTitleTextLabel = Label::createWithTTF(FontUtils::getStrongFontTTFConfig(), floorTitleText);
    floorTitleTextLabel->setPosition(Point(floorTitleCutInLayer->getContentSize().width / 2, floorTitleCutInLayer->getContentSize().height / 2));
    floorTitleCutInLayer->addChild(floorTitleTextLabel);

    this->addChild(modalLayer, ZOrders::CutInLayerZOrder);

    LayerActionUtils::runCutInActionFadeOut(floorTitleCutInLayer, 1.5f, [this, modalLayer]() {
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    modalLayer->addChild(floorTitleCutInLayer);
}

// gameoverカットインを生成する
void RogueScene::playGameOverCutIn() {
    auto winSize = Director::getInstance()->getWinSize();
    auto modalLayer = ModalLayer::create(Color3B::BLACK, 255);

    // TODO: (kyokomi) この画像は一般公開できないので注意
    auto gameOverSprite = Sprite::create("game_over.jpg");
    gameOverSprite->setPosition(Point(winSize.width / 2, winSize.height / 2));
    modalLayer->addChild(gameOverSprite, ZOrders::CutInLayerZOrder);
    this->addChild(modalLayer, ZOrders::ModalLayerZOrder);
    
    LayerActionUtils::runCutInActionFadeInOut(gameOverSprite, 2.0f, [this, modalLayer]() {
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
        // マイページへ
        this->changeScene(TopScene::scene<TopScene>());
    });
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

void RogueScene::changeGameStatus(RoguePlayDto::GameStatus gameStatus) {
    CCLOG("turn %d change gameStatus %d => %d", this->_roguePlayDto.getTurnCount(), this->_gameStatus, gameStatus);
    
    auto pPlayer = getPlayerActorSprite(1);
    
    this->_beforeGameStatus = _gameStatus;
    this->_gameStatus = gameStatus;
    
    if (this->_beforeGameStatus == RoguePlayDto::GameStatus::GAME_OVER ||
        this->_beforeGameStatus == RoguePlayDto::GameStatus::QUEST_CLEAR) {
        return;
    }
    
    switch (this->_gameStatus) {
        case RoguePlayDto::GameStatus::GAME_OVER:
            // セーブ消去
            AccountData::getInstance()->resetRoguePlayDataAndInventory();
            // ゲームオーバーの演出
            playGameOverCutIn();
            return;
        case RoguePlayDto::GameStatus::ESCAPE:
            // セーブ消去
            AccountData::getInstance()->resetRoguePlayData();
            // マイページへ
            changeScene(TopScene::scene<TopScene>());
            return;
        case RoguePlayDto::GameStatus::QUEST_CLEAR:
            // セーブ消去
            AccountData::getInstance()->resetRoguePlayData();
            
            // クリアフラグ更新
            AccountData::getInstance()->clearQuestTypeWithUpdateGamePlayProgress(this->_roguePlayDto.getQuestType());
            
            // クリア演出ADVパートへ(TODO: とりあえずquestType=ノベルIDにしてる)
            changeScene(NovelScene::scene((int)this->_roguePlayDto.getQuestType(), 0, [this]() {
                // ADVパート終わったらマイページへ
                auto trans = TransitionProgressOutIn::create(0.5f, TopScene::scene<TopScene>());
                Director::getInstance()->replaceScene(trans);
            }));
            return;
        default:
            break;
    }
    
    
    if (this->_gameStatus == RoguePlayDto::GameStatus::ENEMY_TURN &&
        (this->_beforeGameStatus == RoguePlayDto::GameStatus::PLAYER_TURN ||
         this->_beforeGameStatus == RoguePlayDto::GameStatus::PLAYER_ACTION ||
         this->_beforeGameStatus == RoguePlayDto::GameStatus::PLAYER_NO_ACTION)) {
        // 敵のターン開始時
        enemyTurn();

    } else if (this->_gameStatus == RoguePlayDto::GameStatus::PLAYER_ACTION) {
        this->_roguePlayDto.resetNoActionCount();
    } else if (this->_gameStatus == RoguePlayDto::GameStatus::PLAYER_NO_ACTION) {
        this->_roguePlayDto.countUpNoAction();
    } else if (this->_gameStatus == RoguePlayDto::GameStatus::PLAYER_TURN) {
        this->getMapManager()->clearCursor();
        // ターン数を進める
        _roguePlayDto.countUpTurn();
        
        // TODO: とりあえずここで・・・
        auto pPlayerDto = pPlayer->getActorDto();
        
        // 10ターンに1空腹度が減るという
        if (_roguePlayDto.getTurnCount() % 10 == 0) {
            pPlayerDto->countDownMagicPoint();
        }
        // 無行動が4ターン続くとHPが回復
        if (_roguePlayDto.getNoActionCount() == 4) {
            _roguePlayDto.resetNoActionCount();
            pPlayerDto->countUpHitPoint();
        }
        
        // 敵のリポップ
        // ランダムなタイミング（毎ターン5%くらい）に湧く
        int rand = GetRandom(1, 100);
        if (rand <= 5) {
            // 1体沸く
            institutionEnemy(1);
        }
    }

    // プレイステータス更新
    refreshStatus();
    // 照明情報更新
    rogueMapLighting();
}

void RogueScene::changeScene(Scene* scene) {
    // 真っ黒のモーダルを載せる
    auto modal = ModalLayer::create();
    modal->setColor(Color3B::BLACK);
    modal->setOpacity(255);
    this->addChild(modal, ZOrders::BlackLayerZOrder);
    
    auto trans = TransitionProgressOutIn::create(1.0f, scene);
    Director::getInstance()->replaceScene(trans);
}

float RogueScene::getAnimationSpeed() {
    if (_keypadLayout->isKeypadDashed()) {
        return 0.0f;
    }
    return 0.2f;
}

#pragma mark
#pragma mark モンスターのAI関連

void RogueScene::enemyTurn() {
    auto rogue_map_layer = getRogueMapLayer();
    
    // モンスターの数だけ繰り返す
    std::list<ActorMapItem> enemyList = this->getMapManager()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList) {
        // TODO: ランダムでとどまるか移動するかきめる
        int rand = GetRandom(2, 2);
        if (rand == 1) {
            auto pEnemySprite = rogue_map_layer->getEnemyActorSprite(enemyMapItem.seqNo);
            // TODO: MapManagerも更新する必要がある
            pEnemySprite->moveDone();
            
            // とどまる
            auto message = cocos2d::StringUtils::format("様子を見ている seqNo = %d", enemyMapItem.seqNo);
            this->logMessage(message);
        } else if (rand == 2) {
            // プレイヤーに向かって移動 or プレイヤーに攻撃
            auto pPlayerActorSprite = getPlayerActorSprite(1);
            
            // プレイヤーの周辺で最もコストが低い箇所へ移動
            auto playerMapIndex = pPlayerActorSprite->getActorMapItem().mapIndex;
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
                moveMapIndex =pPlayerActorSprite->getActorMapItem().mapIndex;
                
            } else {
                // 移動可能な経路情報を設定
                this->getMapManager()->createActorFindDist(enemyMapItem.mapIndex, enemyMapItem.moveDist);
                
                // 最も移動コストがかからない場所を抽出
                MapItem targetMoveDistMapItem = this->getMapManager()->searchTargetMapItem(searchMapIndexList);
                
                // 移動リスト作成
                if (targetMoveDistMapItem.mapDataType == MapDataType::MOVE_DIST) {
                    std::list<MapIndex> moveList = this->getMapManager()->createMovePointList(targetMoveDistMapItem.mapIndex, enemyMapItem);
                    if (moveList.size() > 1) {
                        std::list<MapIndex>::iterator it = moveList.begin();
                        it++;
                        moveMapIndex = *it; // 2件目を取得(1件目は自分なので）
                        it = moveList.end();
                    }
                }
            }
            searchMapIndexList.clear();
            
            // 移動有無関係なく向きは変える
            auto pEnemySprite = rogue_map_layer->getEnemyActorSprite(enemyMapItem.seqNo);
            MapIndex addMoveIndex = {
                moveMapIndex.x - pEnemySprite->getActorMapItem().mapIndex.x,
                moveMapIndex.y - pEnemySprite->getActorMapItem().mapIndex.y,
                this->getMapManager()->checkMoveDirectionType(moveMapIndex, pEnemySprite->getActorMapItem().mapIndex)
            };
            pEnemySprite->runMoveAction(addMoveIndex);
            // 行動前にする
            pEnemySprite->done(false);
            
            if (rogue_map_layer->isMapLayerOver(moveMapIndex)) {
                
                CCLOG("移動不可 seqNo = %d (%d, %d)", enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                pEnemySprite->moveDone();
                
            } else if (rogue_map_layer->isTiledMapColisionLayer(moveMapIndex)) {
                
                auto message = cocos2d::StringUtils::format("壁ドーン seqNo = %d (%d, %d)",
                                                            enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                this->logMessage(message);
                pEnemySprite->moveDone();
                
            } else if (this->getMapManager()->getActorMapItem(moveMapIndex).mapDataType == MapDataType::ENEMY) {
                
                if (MAP_INDEX_DIFF(enemyMapItem.mapIndex, moveMapIndex)) {
                    //logMessage("待機 seqNo = %d (%d, %d)");
                } else {
                    auto message = cocos2d::StringUtils::format("敵ドーン seqNo = %d (%d, %d)",
                                                                enemyMapItem.seqNo, moveMapIndex.x, moveMapIndex.y);
                    this->logMessage(message);
                }
                pEnemySprite->moveDone();
                
            } else if (this->getMapManager()->getActorMapItem(moveMapIndex).mapDataType == MapDataType::PLAYER) {
                // 移動中のステータスへ
                changeGameStatus(RoguePlayDto::GameStatus::ENEMY_ACTION);
                
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
                    auto message = cocos2d::StringUtils::format("%sの攻撃: %sに%dダメージ",
                                                                enemy->getName().c_str(), player->getName().c_str(), damage);
                    this->logMessage(message);
                    // オーバーキル判定
                    player->damageHitPoint(damage);
                    
                    // プレイヤーステータス更新と死亡判定
                    this->refreshStatus();
                    
                    pEnemySprite->attackDone();
                    
                    checkEnmeyTurnEnd();
                    
                }), NULL));
            } else {
                // アニメーション中のステータスへ
                changeGameStatus(RoguePlayDto::GameStatus::ENEMY_ACTION);
                
                // 移動開始
                rogue_map_layer->moveEnemyMap(enemyMapItem.seqNo, addMoveIndex, getAnimationSpeed(),CallFunc::create([this, pEnemySprite]() {
                    // 移動終わり
                    pEnemySprite->moveDone();
                    
                    checkEnmeyTurnEnd();
                }));
            }
        }
    }
    checkEnmeyTurnEnd();
}

void RogueScene::checkEnmeyTurnEnd() {
    bool isTurnEnd = true;
    std::list<ActorMapItem> enemyList = this->getMapManager()->findEnemyMapItem();
    for (ActorMapItem enemyMapItem : enemyList) {
        auto pEnemySprite = getRogueMapLayer()->getEnemyActorSprite(enemyMapItem.seqNo);
        auto pEnemyMapItem = pEnemySprite->getActorMapItem();
        if (!pEnemyMapItem.moveDone && !pEnemyMapItem.attackDone) {
            isTurnEnd = false;
            break;
        }
    }
    if (isTurnEnd) {
        changeGameStatus(RoguePlayDto::GameStatus::PLAYER_TURN);
    } else {
        changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
    }
}

#pragma mark
#pragma mark タッチイベント関連

void RogueScene::touchEventExec(cocos2d::Vec2 touchPoint) {
    
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
    MapIndex add_move_index = MapManager::checkTouchEventIndex(actor_map_item.mapIndex, touch_point_map_index);
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
    
    // 敵をタッチした
    auto enemy_map_item = this->getMapManager()->getActorMapItem(touchPointMapIndex);
    if (enemy_map_item.mapDataType == MapDataType::ENEMY) {
        // 向きだけ変えてターン経過しない
        return;
    }
    
    // 障害物判定
    if (rogue_map_layer->isTiledMapColisionLayer(touchPointMapIndex)) {
        // TODO: ぶつかるSE再生
        this->logMessage("壁ドーン SE再生");
        
        // ターン経過なし
        
    } else {
        
        changeGameStatus(RoguePlayDto::GameStatus::PLAYER_NO_ACTION);

        auto touch_point_map_item = this->getMapManager()->getMapItem(touchPointMapIndex);
        
        // 移動処理
        rogue_map_layer->movePlayerMap(actor_sprite, addMoveIndex, getAnimationSpeed(), CallFunc::create([this, touch_point_map_item](void) {
            
            // アイテムに重なったときの拾う処理
            if (touch_point_map_item.mapDataType == MapDataType::MAP_ITEM) {
                // item
                auto dropMapItem = this->getMapManager()->getDropMapItem(touch_point_map_item.mapIndex);
                this->touchDropItem(dropMapItem);
                
            } else if (touch_point_map_item.mapDataType == MapDataType::KAIDAN) {
                // 階段
                this->touchKaidan();
            }
            
            // ターンエンド
            changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
        }));
        
        // コールバックまでgameStatusを更新はしない
    }
}

ItemDto RogueScene::touchDropItem(const DropMapItem& drop_map_item, bool isDropMessage /* = true */) {
    auto rogue_map_layer = getRogueMapLayer();
    
    // itemを取得
    auto drop_item_sprite = rogue_map_layer->getDropItemSprite(drop_map_item.seqNo);
    auto itemDto = drop_item_sprite->getItemDto();
    
    std::string message;
    
    // ゴールドは別扱い
    if (itemDto.getItemType() == MUseItem::ItemType::GOLD) {
        
        // TODO: (kyokomi) 拾うSE再生
        
        // メッセージログ
        message = cocos2d::StringUtils::format("%d%sを拾った。", itemDto.getParam(), itemDto.getName().c_str());
        
        // ゴールドを加算
        this->_itemInventory.addGold(itemDto.getParam());
        
        // Map上から削除する
        rogue_map_layer->removeDropItemSprite(drop_item_sprite);
        
    } else {
        // ゴールド以外はインベントリへ
        
        // イベントリに追加する
        if (this->_itemInventory.addItemDto(itemDto)) {
            message = itemDto.createItemName() + "を拾った。";
            
            // TODO: (kyokomi) 拾うSE再生
            
            // Map上から削除する
            rogue_map_layer->removeDropItemSprite(drop_item_sprite);
        } else {
            // アイテム所持数限界
            message = "持ち物が一杯で、\n" + itemDto.createItemName() + "を拾えなかった。";
        }
    }
    
    if (isDropMessage) {
        this->logMessage(message);
    }
    
    return itemDto;
}

void RogueScene::touchKaidan() {
    
    // TODO: (kyokomi)階段SE
    
    // 階段下りる判定
    Size win_size = Director::getInstance()->getWinSize();
    auto alertDialog = AlertDialogLayer::createWithContentSizeModal(win_size * 0.5, "階段です。\n　\n次の階に進みますか？", "は　い", [this](Ref *ref) {
        // save
        auto actor_sprite = getPlayerActorSprite(1);
        
        int nextQuestId = this->_roguePlayDto.getQuestId() + 1;
        this->_roguePlayDto.setQuestId(nextQuestId);
        
        ActorDto playerActor = *(actor_sprite->getActorDto());
        AccountData::getInstance()->save(this->_roguePlayDto,
                                         playerActor,
                                         this->_itemInventory);
        
        auto questData = RogueGameConfig::getQuestData(RoguePlayDto::findQuestKey(this->_roguePlayDto.getQuestType()));
        int clearCount = questData.at("clearCount").asInt();
        if (clearCount < nextQuestId) {
            // クエストクリア
            this->changeGameStatus(RoguePlayDto::GameStatus::QUEST_CLEAR);
        } else {
            // 画面遷移
            this->changeScene(RogueScene::scene(this->_roguePlayDto.getQuestType(), nextQuestId));
        }
    }, "いいえ", [](Ref *ref) {});
    this->addChild(alertDialog, ZOrders::ModalLayerZOrder);
}

void RogueScene::attack() {
    auto rogue_map_layer = getRogueMapLayer();
    auto pActorSprite = getPlayerActorSprite(1);
    
    MapIndex addMapIndex = {0, 0, MoveDirectionType::MOVE_NONE};
    MapIndex attackMapIndex = pActorSprite->getActorMapItem().mapIndex;
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
    
    changeGameStatus(RoguePlayDto::GameStatus::PLAYER_ACTION);
    
    // 攻撃アニメーション
    float speed = getAnimationSpeed();
    auto pMove1 = MoveTo::create(speed, pActorSprite->getPosition() + rogue_map_layer->indexToPointNotTileSize(addMapIndex.x, addMapIndex.y));
    auto pMove2 = MoveTo::create(speed, pActorSprite->getPosition());

    ActorSprite* pEnemySprite = NULL;
    // 敵をタッチした
    auto pEnemyMapItem = this->getMapManager()->getActorMapItem(attackMapIndex);
    if (pEnemyMapItem.mapDataType == MapDataType::ENEMY) {
        pEnemySprite = rogue_map_layer->getEnemyActorSprite(pEnemyMapItem.seqNo);
    } else {
        // 空振り
    }
    
    // アニメーション
    pActorSprite->runAction(Sequence::create(pMove1, pMove2, CallFunc::create([this, pActorSprite, pEnemySprite](void) {
        if (pEnemySprite) {
            this->attackCallback(pActorSprite, pEnemySprite);
        }
        changeGameStatus(RoguePlayDto::GameStatus::ENEMY_TURN);
    }), NULL));
}

void RogueScene::attackCallback(ActorSprite* pActorSprite, ActorSprite* pEnemySprite) {
    auto player = pActorSprite->getActorDto();
    auto enemy = pEnemySprite->getActorDto();
    
    // 攻撃開始
    int damage = BattleLogic::exec(*player, *enemy);
    // 攻撃イベント
    auto message = cocos2d::StringUtils::format("%sの攻撃: %sに%dのダメージ",
                                                player->getName().c_str(), enemy->getName().c_str(), damage);
    this->logMessage(message);
    
    // 敵の死亡判定
    bool isDead = enemy->damageHitPoint(damage);
    if (isDead) {
        
        auto message = cocos2d::StringUtils::format("%sを倒した。経験値%dを得た。",
                                                    enemy->getName().c_str(), enemy->getExp());
        this->logMessage(message);
        // TODO: (kyokomi) 経験値更新（計算式 適当）
        if (player->growExpAndLevelUpCheck(enemy->getExp())) {
            
            // TODO: レベルアップ演出（SE？）
            auto message = cocos2d::StringUtils::format("%sはレベル%dになった。",
                                                        player->getName().c_str(), player->getLv());
            this->logMessage(message);
            
            // レベル上がってステータスが上がるかもしれないので攻撃力、防御力のステータスを更新する
            this->refreshStatusEquip(*player);
        }
        
        // 敵のドロップ確率を抽選してアイテムの抽選（TODO: とりあえず一律3%）
        if (LotteryUtils::isHit(300)) {
            // アイテムdrop
            institutionDropItem(1, pEnemySprite->getActorMapItem().mapIndex);
        }
        // マップから消える
        getRogueMapLayer()->removeEnemyActorSprite(pEnemySprite);
    }
}

#pragma mark
#pragma mark UI関連

void RogueScene::logMessage(const std::string& messageText)
{
    this->_gameLogWidget->logMessage(messageText.c_str());
}

void RogueScene::showSystemMenu() {
    
    auto systemMenuModalLayer = dynamic_cast<ModalLayer*>(this->getChildByTag(Tags::SystemMenuWindowTag));
    if (systemMenuModalLayer) {
        systemMenuModalLayer->setVisible(true);
    } else {
        systemMenuModalLayer = ModalLayer::create();
        Size win_size = Director::getInstance()->getWinSize();
        std::list<SystemMenuLayer::SystemMenuButtonInfo> menuButtonInfoList;
        
        // TODO: (kyokomi) デバッグ用
        SystemMenuLayer::SystemMenuButtonInfo menu1("千里眼", [this, systemMenuModalLayer]() {
            CCLOG("Menu1ボタンが押された！");
            
            this->logMessage("千里眼を発動した。");
            
            this->itemMappingAllShow();
            
            this->hideSystemMenu();
        });
        
        // TODO: (kyokomi) デバッグ用
        SystemMenuLayer::SystemMenuButtonInfo menu2("地獄耳", [this, systemMenuModalLayer]() {
            CCLOG("Menu2ボタンが押された！");
            
            this->logMessage("地獄耳を発動した。");
            
            this->enemyMappingAllShow();
            
            this->hideSystemMenu();
        });
        
        // TODO: (kyokomi) デバッグ用
        SystemMenuLayer::SystemMenuButtonInfo menu3("階　段", [this, systemMenuModalLayer]() {
            CCLOG("Menu3ボタンが押された！");
            this->touchKaidan();
            
            this->hideSystemMenu();
        });
        
        SystemMenuLayer::SystemMenuButtonInfo menu4("脱　出", [this, systemMenuModalLayer]() {
            CCLOG("Menu4ボタンが押された！");
            Size win_size = Director::getInstance()->getWinSize();
            auto alertDialog = AlertDialogLayer::createWithContentSizeModal(win_size * 0.5, "あきらめますか？", "は　い", [this](Ref *ref) {
                // ゲームオーバー
                this->changeGameStatus(RoguePlayDto::GameStatus::GAME_OVER);
                
            }, "いいえ", [this](Ref *ref) {
                this->showSystemMenu();
            });
            this->addChild(alertDialog, ZOrders::ModalLayerZOrder);
            this->hideSystemMenu();
        });
        
        // TODO: (kyokomi) 足元処理未実装
        SystemMenuLayer::SystemMenuButtonInfo menu5("足　元", [this, systemMenuModalLayer]() {
            CCLOG("Menu5ボタンが押された！");
            
            this->logMessage("足元は何もありません。");
            
            this->hideSystemMenu();
        });
        
        menuButtonInfoList.push_back(menu1);
        menuButtonInfoList.push_back(menu2);
        menuButtonInfoList.push_back(menu3);
        menuButtonInfoList.push_back(menu4);
        menuButtonInfoList.push_back(menu5);
        menuButtonInfoList.push_back(SystemMenuLayer::SystemMenuButtonInfo("明かり", [this, systemMenuModalLayer]() {
            CCLOG("Menu6ボタンが押された！");
            this->floorMappingAllShow();
        }));
        menuButtonInfoList.push_back(SystemMenuLayer::SystemMenuButtonInfo("持ち帰", [this, systemMenuModalLayer]() {
            CCLOG("Menu7ボタンが押された！");
            
            this->changeGameStatus(RoguePlayDto::GameStatus::ESCAPE);
            
            this->hideSystemMenu();
        }));
        menuButtonInfoList.push_back(SystemMenuLayer::SystemMenuButtonInfo("未設定", [this, systemMenuModalLayer]() {
            CCLOG("Menu8ボタンが押された！");
            this->hideSystemMenu();
        }));
        menuButtonInfoList.push_back(SystemMenuLayer::SystemMenuButtonInfo("とじる", [this, systemMenuModalLayer]() {
            CCLOG("Menu9ボタンが押された！");
            this->hideSystemMenu();
        }));
        
        auto systemMenuLayer = SystemMenuLayer::create(win_size * 0.6, "その他・システムメニュー");
        systemMenuLayer->setPosition(CommonWindowUtil::createPointCenter(systemMenuLayer, systemMenuModalLayer));
        systemMenuLayer->setMenuButtonList(menuButtonInfoList);
        
        systemMenuModalLayer->addChild(systemMenuLayer);
        this->addChild(systemMenuModalLayer, ZOrders::ModalLayerZOrder, Tags::SystemMenuWindowTag);
    }
}

void RogueScene::hideSystemMenu() {
    auto systemMenuModalLayer = dynamic_cast<ModalLayer*>(this->getChildByTag(Tags::SystemMenuWindowTag));
    if (systemMenuModalLayer) {
        systemMenuModalLayer->setVisible(false);
    }
}

void RogueScene::refreshStatus()
{
    if (!_statusWidget) {
        return;
    }
    
    auto pPlayerSprite = getPlayerActorSprite(1);
    auto pPlayerDto = pPlayerSprite->getActorDto();

    int questId = _roguePlayDto.getQuestId(); // フロア情報（クエストID=フロア数でいい？)
    _statusWidget->setStatus(questId, *pPlayerDto, this->_itemInventory.getGold());
    
    // TODO: 死亡判定ここで？
    if (pPlayerDto->getHitPoint() == 0) {
        auto message = cocos2d::StringUtils::format("%sは死亡した。", pPlayerDto->getName().c_str());
        this->logMessage(message);
        changeGameStatus(RoguePlayDto::GameStatus::GAME_OVER);
    } else if (pPlayerDto->getMagicPoint() == 0) {
        auto message = cocos2d::StringUtils::format("%sは空腹で倒れた。", pPlayerDto->getName().c_str());
        this->logMessage(message);
        changeGameStatus(RoguePlayDto::GameStatus::GAME_OVER);
    } else {
        // 残りHPで文字色を変える
        float hitPointDiv = (float)pPlayerDto->getHitPoint() / (float)pPlayerDto->getHitPointLimit();
        float mpDiv = (float)pPlayerDto->getMagicPoint() / (float)pPlayerDto->getMagicPointLimit();
        if (hitPointDiv <= 0.25 || mpDiv <= 0.10) {
            _statusWidget->setStatusColor(Color3B::RED);
            _gameLogWidget->setStatusColor(Color3B::RED);
        } else if (hitPointDiv <= 0.50 || mpDiv <= 0.30) {
            _statusWidget->setStatusColor(Color3B::YELLOW);
            _gameLogWidget->setStatusColor(Color3B::YELLOW);
        } else {
            _statusWidget->setStatusColor(Color3B::WHITE);
            _gameLogWidget->setStatusColor(Color3B::WHITE);
        }
    }
}

// 装備状態更新
void RogueScene::refreshStatusEquip(const ActorDto& actorDto)
{
    if (_statusWidget) {
        _statusWidget->setEquip(actorDto);
    }
}

#pragma mark
#pragma mark 照明

void RogueScene::rogueMapLighting() {
    
    auto actor_sprite = getPlayerActorSprite(1);
    auto rogue_map_layer = getRogueMapLayer();
    
    // プレイヤー周辺３＊３はかならず更新
    // プレイヤー視野でマップをリフレッシュする
    rogue_map_layer->refreshPlayerRectAutoMapping(actor_sprite->getActorMapItem().mapIndex);
    
    Rect floorInfoIndexRect = rogue_map_layer->getTileMapFloorInfo(actor_sprite->getActorMapItem().mapIndex);
    if (floorInfoIndexRect.equals(Rect::ZERO)) {
        // 視野をプレイヤー周辺に更新
        rogue_map_layer->hideFloorLighting();
        showPlayerLighting(actor_sprite);
    } else {
        // 視野を部屋に更新
        hidePlayerLighting();
        
        rogue_map_layer->showFloorLighting(actor_sprite->getActorMapItem().mapIndex, floorInfoIndexRect);
        
        // マッピングとマップ情報も更新
        rogue_map_layer->refreshFloorRectAutoMapping(floorInfoIndexRect);
    }
}

void RogueScene::showPlayerLighting(ActorSprite* actor_sprite) {
    
    auto winSize = Director::getInstance()->getWinSize();
    
    auto base_layer = dynamic_cast<LayerColor*>(this->getChildByTag(Tags::RoguePlayerLightBaseTag));
    if (base_layer) {
        base_layer->setVisible(true);
    } else {
        base_layer = LayerColor::create();
        this->addChild(base_layer, ZOrders::RoguePlayerLightBaseZOrder, Tags::RoguePlayerLightBaseTag);
    }
    
    // フロントレイヤーにマップを暗くするやつを設定
    auto mask_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(Tags::RoguePlayerLightMaskTag));
    if (mask_layer) {
        mask_layer->setVisible(true);
    } else {
        mask_layer = LayerColor::create(Color4B(0,0,0,128));
        mask_layer->setContentSize(winSize);
        mask_layer->setPosition(Point::ZERO);
        base_layer->addChild(mask_layer, ZOrders::RoguePlayerLightMaskZOrder, Tags::RoguePlayerLightMaskTag);
    }
    
    auto light_draw = dynamic_cast<DrawNode*>(mask_layer->getChildByTag(Tags::RoguePlayerLightTag));
    if (light_draw) {
        light_draw->setVisible(true);
    } else {
        light_draw = DrawNode::create();
        light_draw->drawDot(actor_sprite->getPosition(), getRogueMapLayer()->getTileSize().width * 3.0f / 2.0f, Color4F::WHITE);
        mask_layer->addChild(light_draw, ZOrders::RoguePlayerLightZOrder, Tags::RoguePlayerLightTag);
        
        BlendFunc blend;
        blend.src = GL_DST_COLOR;
        blend.dst = GL_ONE;
        light_draw->setBlendFunc(blend);
    }
}

void RogueScene::hidePlayerLighting() {
    auto rogue_player_light_base_layer = this->getChildByTag(Tags::RoguePlayerLightBaseTag);
    if (rogue_player_light_base_layer) {
        rogue_player_light_base_layer->setVisible(false);
    }
}

#pragma mark
#pragma mark 呪文

void RogueScene::enemyMappingAllShow() {
    getRogueMapLayer()->setEnemyAllShow(true);
    rogueMapLighting();
}

void RogueScene::itemMappingAllShow() {
    getRogueMapLayer()->setItemAllShow(true);
    rogueMapLighting();
}

void RogueScene::floorMappingAllShow() {
    getRogueMapLayer()->setItemAllShow(true);
    getRogueMapLayer()->setEnemyAllShow(true);
    getRogueMapLayer()->refreshAllFloorMapping();
    rogueMapLighting();
}

#pragma mark
#pragma mark 配置

// モンスター配置
void RogueScene::institutionEnemy(int probCount) {
    ValueMap rogueMapDatas = getRogueMapData();
    
    ValueVector probList = rogueMapDatas.at(RogueGameConfig::MobIds).asValueVector();
    std::vector<int> hitIds = LotteryUtils::lot(probCount, probList);
    if (hitIds.size() <= 0) {
        hitIds.clear();
        return;
    }
    
    auto tiled_map_layer = getRogueMapLayer();
    
    for (int hitId : hitIds) {
        std::string hitIdStr = cocos2d::StringUtils::format("%d", hitId);
        ActorDto enemy_dto = ActorDto::createActorDto(m_monster::data_.at(hitIdStr).asString());
        MapIndex enemyMapIndex = tiled_map_layer->getFloorRandomMapIndex(true);
        tiled_map_layer->tileSetEnemyActorMapItem(enemy_dto, enemyMapIndex);
    }
    hitIds.clear();
}

// アイテム配置
void RogueScene::institutionDropItem(int probCount, const MapIndex& mapIndex /* = MapManager::createMapIndexEmpty() */) {
    ValueMap rogueMapDatas = getRogueMapData();
    
    ValueVector probList = rogueMapDatas.at(RogueGameConfig::DropItemIds).asValueVector();
    ValueVector hitValues = LotteryUtils::lotValues(probCount, probList);
    if (hitValues.size() <= 0) {
        hitValues.clear();
        return;
    }
    
    auto tiled_map_layer = getRogueMapLayer();
    
    for (Value hitValue : hitValues) {
        
        ValueMap valueMap = hitValue.asValueMap();
        
        MUseItem::ItemType itemType = static_cast<MUseItem::ItemType>(valueMap.at(RogueGameConfig::ItemType).asInt());
        int hitId = valueMap.at(RogueGameConfig::Id).asInt();
        
        // 武器、防具の+値やゴールド値を決定する
        int param = 0;
        if (itemType == MUseItem::ItemType::GOLD) {
            int gold = GetRandom(rogueMapDatas.at(RogueGameConfig::GoldMin).asInt(),
                                 rogueMapDatas.at(RogueGameConfig::GoldMax).asInt());
            param = gold;
        } else if (itemType == MUseItem::ItemType::EQUIP_WEAPON || itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
            auto dropItemUpValueLimits = getRogueMapData().at(RogueGameConfig::DropItemUpValueLimits).asValueVector();
            param = LotteryUtils::lot(dropItemUpValueLimits);
        }
        
        if (itemType == MUseItem::ItemType::USE_ITEM || itemType == MUseItem::ItemType::GOLD) {
            // 消費アイテム  or お金
            
            // Master取得
            MUseItem mUseItem = MUseItemDao::getInstance()->selectById(hitId);
            // ドロップアイテム情報を生成
            long objectId = AccountData::getInstance()->getGameObjectId();
            ItemDto itemDto(
                objectId,
                mUseItem.getUseItemId(),
                mUseItem.getUseItemType(),
                mUseItem.getUseItemImageId(),
                mUseItem.getUseItemName(),
                false,
                param
            );
            
            // 配置
            tiled_map_layer->tileSetDropMapItem(itemDto, mapIndex);
            
            // objectIdを更新
            AccountData::getInstance()->gameObjectCountUp();
            
        } else if (itemType == MUseItem::ItemType::EQUIP_WEAPON) {
            // 武器
            MWeapon mWeapon = MWeaponDao::getInstance()->selectById(hitId);
            
            long objectId = AccountData::getInstance()->getGameObjectId();
            ItemDto itemDto(
                objectId,
                mWeapon.getWeaponId(),
                MUseItem::ItemType::EQUIP_WEAPON,
                mWeapon.getWeaponImageId(),
                mWeapon.getWeaponName(),
                false,
                param
            );
            
            tiled_map_layer->tileSetDropMapItem(itemDto, mapIndex);
            
            // objectIdを更新
            AccountData::getInstance()->gameObjectCountUp();
            
        } else if (itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
            // 防具
            MAccessory mAccessory = MAccessoryDao::getInstance()->selectById(hitId);
            
            long objectId = AccountData::getInstance()->getGameObjectId();
            ItemDto itemDto(
                objectId,
                mAccessory.getAccessoryId(),
                MUseItem::ItemType::EQUIP_ACCESSORY,
                mAccessory.getAccessoryImageId(),
                mAccessory.getAccessoryName(),
                false,
                param
            );
            
            tiled_map_layer->tileSetDropMapItem(itemDto, mapIndex);
            
            // objectIdを更新
            AccountData::getInstance()->gameObjectCountUp();
        }
    }
    hitValues.clear();
}

#pragma mark
#pragma mark 汎用

// ローグマップ基本データ取得
const ValueMap RogueScene::getRogueMapData() {
    // フロア情報のIndexを用意する（データがない場合は最終データで補正）
    long questIndex = _roguePlayDto.getQuestId() - 1;
    auto questData = RogueGameConfig::getQuestData(RoguePlayDto::findQuestKey(_roguePlayDto.getQuestType()));
    auto datas = questData.at("floor").asValueVector();
    if (datas.size() <= questIndex) {
        questIndex = datas.size() - 1;
    }
    Value rogueMapData = datas[questIndex];
    return rogueMapData.asValueMap();
}

ActorSprite* RogueScene::getPlayerActorSprite(int seqNo)
{
    return static_cast<ActorSprite*>(getChildByTag(Tags::ActorPlayerTag + seqNo));
}

MapManager* RogueScene::getMapManager()
{
    return getRogueMapLayer()->getMapManager();
}

RogueTMXTiledMap* RogueScene::getRogueMapLayer() {
    return static_cast<RogueTMXTiledMap*>(this->getChildByTag(Tags::TiledMapLayerTag));
}

NS_ROGUE_END

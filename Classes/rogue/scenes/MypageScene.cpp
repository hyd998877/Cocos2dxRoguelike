//
//  MypageScene.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/23.
//
//

#include "MypageScene.h"
#include "AppMacros.h"

#include "CommonWindowUtil.h"
#include "AlertDialogLayer.h"

#include "NovelScene.h"
#include "RogueScene.h"
#include "BattleScene.h"
#include "SRPGScene.h"

#include "SystemMenuLayer.h"
#include "ItemInventoryLayer.h"
#include "ItemLogic.h"
#include "MUseItemDao.h"

#include "AccountData.h"

USING_NS_CC;

NS_ROGUE_BEGIN

MypageScene::MypageScene()
: _itemInventory()
, _itemInventoryStock()
, _baseLayer(nullptr)
, _baseItemDto()
, _materialItemDto()
{
}

MypageScene::~MypageScene()
{
}

Scene* MypageScene::scene()
{
    Scene *scene = Scene::create();
    MypageScene *layer = MypageScene::create();
    scene->addChild(layer);
    return scene;
}

bool MypageScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    // セーブあり
    if (AccountData::getInstance()->isQuestSaveData()) {
        initQuestSave();
    } else {
        initMyPage();
    }

    return true;
}

void MypageScene::initQuestSave()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    std::string saveTitle = "前回プレイしたデータが残っています。\n再開しますか？";
    std::string saveDetail = AccountData::getInstance()->createQuestSaveDetailText();
    std::string dialogTitle = saveTitle + "\n\n" + saveDetail + "\n\n※いいえを選択すると、\n所持していたアイテムは消えます";
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(dialogTitle, "はい", [](Ref *ref) {
        auto scene = RogueScene::scene(AccountData::getInstance()->getRoguePlayData().quest_id);
        auto trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    }, "いいえ", [this](Ref *ref) {
        AccountData::getInstance()->resetRoguePlayData();
        this->initMyPage();
    });
    this->addChild(dialogLayer);
}

void MypageScene::initMyPage()
{
    if (this->_baseLayer) {
        this->_baseLayer->removeAllChildrenWithCleanup(true);
    }
    this->_baseLayer = Layer::create();

    Size win_size = Director::getInstance()->getWinSize();
    
    // タイトル表示
    initHeaderTitle("とっぷぺーじ", "bg/saloon06_c.jpg");
    
    // キャラ表示
    {
        auto actor_sprite = Sprite::create("novel/actor4_novel_s_0.png");
        actor_sprite->setPosition(Point(win_size.width * 0.25, actor_sprite->getContentSize().height * 0.25));
        actor_sprite->setFlippedX(true);
        this->_baseLayer->addChild(actor_sprite);
    }
    
    // セリフ表示
    {
        auto comment_layer = LayerColor::create(Color4B::BLACK);
        comment_layer->setContentSize(Size(win_size.width / 2, win_size.height / 3));
        comment_layer->setPosition(Point(win_size.width * 0.7 - comment_layer->getContentSize().width / 2, win_size.height * 0.5 - comment_layer->getContentSize().height / 2));
        this->_baseLayer->addChild(comment_layer);
        
        // TODO: なんかランダムなメッセージとかにしたいです
        std::string messageText = "今日も気合いれてダンジョンいこー";
        auto gameProgress = AccountData::getInstance()->getGamePlayProgress();
        if (gameProgress == AccountData::GamePlayProgress::INIT) {
            messageText = "はじめまして！\nまずはクエストをやってみよう！";
        }
        
        auto comment_label = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), messageText);
        comment_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
        comment_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
        comment_layer->addChild(comment_label);
        comment_label->setPosition(Point(comment_layer->getContentSize().width / 2, comment_layer->getContentSize().height / 2));
        
        // 枠
        CommonWindowUtil::attachWindowWaku(comment_layer);
    }
    
    // グロナビ生成
    initGlobalMenu(GlobalMenuTags::TOP);
    
    this->addChild(this->_baseLayer);
}

void MypageScene::initItemStockPage()
{
    if (this->_baseLayer) {
        this->_baseLayer->removeAllChildrenWithCleanup(true);
    }
    this->_baseLayer = Layer::create();
    
    // タイトル表示
    initHeaderTitle("そうこ", "bg/ci_shopbakery01a_b.jpg");
    
    {
        this->showItemStockWindow();
    }
    
    // グロナビ生成
    initGlobalMenu(GlobalMenuTags::STOCK);
    
    this->addChild(this->_baseLayer);
}

void MypageScene::initMixedPage()
{
    if (this->_baseLayer) {
        this->_baseLayer->removeAllChildrenWithCleanup(true);
    }
    this->_baseLayer = Layer::create();
    
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    
    // タイトル表示
    initHeaderTitle("ごうせい屋", "bg/shop03_a.jpg");
    
    {
        Size win_size = Director::getInstance()->getWinSize();
        Size layerSize = Size(win_size.width*0.5, win_size.height*0.3);
        std::string spriteFrameFileName = "grid32.png";
        
        auto imageSpriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameFileName);
        if (!imageSpriteFrame) {
            imageSpriteFrame = cocos2d::Sprite::create(spriteFrameFileName)->getSpriteFrame();
            cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(imageSpriteFrame, spriteFrameFileName);
        }
        
        // ベース
        auto baseSprite = cocos2d::Sprite::createWithSpriteFrame(imageSpriteFrame);
        auto baseItemMenuLabel = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, baseSprite, FontUtils::getDefaultFontTTFConfig(), "ベースを選択してください", [this, baseSprite](Ref *ref) {
            auto label = static_cast<Label*>(static_cast<MenuItemLabel*>(ref)->getLabel());
            showMixedItemSelectWindow([this, label, baseSprite](const ItemDto &itemDto) {
                auto itemSprite = Sprite::create(ItemLogic::createItemImageFileName(itemDto.getImageResId()));
                baseSprite->setTexture(itemSprite->getTexture());
                label->setString(itemDto.createItemName());
                
                this->_baseItemDto = itemDto;
            });
        });
        baseItemMenuLabel->setPosition(Point(win_size.width*0.3, win_size.height*0.7));
        
        // 素材
        auto materialSprite = cocos2d::Sprite::createWithSpriteFrame(imageSpriteFrame);
        auto materialItemlayer = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, materialSprite, FontUtils::getDefaultFontTTFConfig(), "そざいを選択してください",[this, materialSprite](Ref *ref) {
            auto label = static_cast<Label*>(static_cast<MenuItemLabel*>(ref)->getLabel());
            showMixedItemSelectWindow([this, label, materialSprite](const ItemDto &itemDto) {
                auto itemSprite = Sprite::create(ItemLogic::createItemImageFileName(itemDto.getImageResId()));
                materialSprite->setTexture(itemSprite->getTexture());
                label->setString(itemDto.createItemName());
                
                this->_materialItemDto = itemDto;
            });
        });
        materialItemlayer->setPosition(Point(win_size.width*0.3, win_size.height*0.3));
        
        // L表示
        auto sprite1 = Sprite::create("ui/l_image.png");
        sprite1->setPosition(Point(win_size.width*0.6, win_size.height*0.65));
        this->_baseLayer->addChild(sprite1);
        auto sprite2 = Sprite::create("ui/l_image.png");
        sprite2->setFlippedY(true);
        sprite2->setPosition(Point(win_size.width*0.6, win_size.height*0.35));
        this->_baseLayer->addChild(sprite2);
        
        // 合成ボタン
        auto mixedMenuItem = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getStrongFontTTFConfig(), "合　成"), Size(16, 8), [this](Ref *ref) {
            // 合成実行
            if (this->mixedItem()) {
                // 成功したらベースと素材を未選択にする
                this->initMixedPage();
            }
        });
        mixedMenuItem->setPosition(Point(win_size.width*0.7, win_size.height*0.5));
        
        auto menu = Menu::create(baseItemMenuLabel, materialItemlayer, mixedMenuItem, NULL);
        menu->setPosition(Point::ZERO);
        this->_baseLayer->addChild(menu);
    }
    
    // グロナビ生成
    initGlobalMenu(GlobalMenuTags::MIXED);
    
    this->addChild(this->_baseLayer);
}

// initXXXXPage後によぶ
void MypageScene::initGlobalMenu(int selectMenuTag)
{
    auto menu = createGlobalMenu();
    
    auto item_menu1 = menu->getChildByTag(selectMenuTag);
    item_menu1->setColor(Color3B::GREEN);
    
    auto black_layer = LayerColor::create(Color4B::BLACK);
    black_layer->setOpacity(192);
    black_layer->setContentSize(Size(menu->getContentSize().width, item_menu1->getContentSize().height + 8));
    this->_baseLayer->addChild(black_layer);
    
    this->_baseLayer->addChild(menu);
}
// initXXXXPage後によぶ
void MypageScene::initHeaderTitle(const std::string &titleText, const std::string &backgroundName)
{
    Size win_size = Director::getInstance()->getWinSize();

    // 背景
    if (!backgroundName.empty()) {
        auto background = Sprite::create(backgroundName);
        background->setOpacity(96);
        background->setScale(win_size.width/background->getContentSize().width);
        background->setPosition(Point(win_size.width/2, win_size.height/2));
        this->_baseLayer->addChild(background);
    }
    
    // タイトル表示
    auto title_layer = LayerColor::create(Color4B::BLACK);
    title_layer->setContentSize(Size(win_size.width, win_size.height / 8));
    title_layer->setPosition(Point(0, win_size.height - title_layer->getContentSize().height));
    this->_baseLayer->addChild(title_layer);
    
    auto title_label = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), titleText);
    title_label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    title_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    title_label->setPosition(Point(title_layer->getContentSize().width /2, title_layer->getContentSize().height / 2));
    title_layer->addChild(title_label);
    
    // 枠
    auto window_waku = CommonWindowUtil::createWindowWaku(Size(win_size.width, title_label->getContentSize().height + title_label->getSystemFontSize()));
    title_layer->addChild(window_waku);
}

/**
 * グロナビ生成
 */
Menu* MypageScene::createGlobalMenu()
{
    // メニュー
    const Size WAKU_PADDING = Size(8, 8);
    
    auto item_menu1 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "と　っ　ぷ"), WAKU_PADDING, [this](Ref *ref) {
        this->initMyPage();
    });
    item_menu1->setTag(GlobalMenuTags::TOP);
    
    auto item_menu2 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "くえ　すと"), WAKU_PADDING, [this](Ref *ref) {
        CCLOG("tappedMenuItem3");
        
        if (AccountData::getInstance()->getGamePlayProgress() == AccountData::GamePlayProgress::INIT) {
            Scene* scene = NovelScene::scene(2, 0, [this]() {
                CCLOG("novel2 end");
                
                AccountData::getInstance()->updateGamePlayProgress(AccountData::GamePlayProgress::TUTORIAL_PLAY);
                int play_quest_id = 1;
                auto scene = RogueScene::scene(play_quest_id);
                auto trans = TransitionProgressOutIn::create(1, scene);
                Director::getInstance()->replaceScene(trans);
            });
            TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        } else {
            int play_quest_id = 1;
            auto scene = RogueScene::scene(play_quest_id);
            auto trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        }
    });
    item_menu2->setTag(GlobalMenuTags::QUEST);
    
    auto item_menu3 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "そ　う　こ"), WAKU_PADDING, [this](Ref *ref) {
        this->initItemStockPage();
    });
    item_menu3->setTag(GlobalMenuTags::STOCK);
    
    auto item_menu4 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "の　べ　る"), WAKU_PADDING, [this](Ref *ref) {
        Scene* scene = NovelScene::scene(1, 0, [this]() {
            CCLOG("novel end");
            Scene* scene = MypageScene::scene();
            TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    item_menu4->setTag(GlobalMenuTags::NOVEL);
    
    auto item_menu5 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "ごう　せい"), WAKU_PADDING, [this](Ref *ref) {
        this->initMixedPage();
    });
    item_menu5->setTag(GlobalMenuTags::MIXED);
    
    auto item_menu6 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "て　す　と"), WAKU_PADDING, [this](Ref *ref) {
        this->showDebugMenu();
    });
    item_menu6->setTag(GlobalMenuTags::TEST);
    
    auto menu = Menu::create(item_menu1, item_menu2, item_menu3, item_menu4, item_menu5, item_menu6, NULL);
    // 4間隔で横に並べる
    menu->alignItemsHorizontallyWithPadding(4);
    menu->setPosition(Point(menu->getContentSize().width / 2, item_menu1->getContentSize().height / 2 + WAKU_PADDING.height / 2));
    
    return menu;
}

void MypageScene::showItemStockWindow()
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    this->_itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
    this->_itemInventoryStock.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    std::list<ItemInventoryDto> itemInventoryList{
        this->_itemInventory,
        this->_itemInventoryStock
    };
    
    auto itemWindow = ItemInventoryLayer::create(itemInventoryList);
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_DROP, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("本当に捨ててもいいですか？", "は　い", [this, itemDto, itemWindow](Ref *ref) {
                    
                    auto saleInventory = ItemInventoryDto::findByObjectIdInventory(itemDto.getObjectId(),
                                                                                   &this->_itemInventory,
                                                                                   &this->_itemInventoryStock);
                    saleInventory->removeItemDto(itemDto.getObjectId());
                    
                    itemWindow->refresh(std::list<ItemInventoryDto>{
                        this->_itemInventory,
                        this->_itemInventoryStock
                    }, *saleInventory);
                }, "いいえ", [](Ref *ref) {});
                this->addChild(dialogLayer, ZOrders::Dialog);
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_SALE, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                int gold = ItemLogic::sale(itemDto);
                auto text = GameCore::StringUtils::format("本当に売ってもいいですか？\n\n金額 %d", gold);
                auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(text, "は　い", [this, gold, itemDto, itemWindow](Ref *ref) {
                    auto saleInventory = ItemInventoryDto::findByObjectIdInventory(itemDto.getObjectId(),
                                                                                   &this->_itemInventory,
                                                                                   &this->_itemInventoryStock);
                    saleInventory->removeItemDto(itemDto.getObjectId());
                    saleInventory->addGold(gold);
                    
                    itemWindow->refresh(std::list<ItemInventoryDto>{
                        this->_itemInventory,
                        this->_itemInventoryStock
                    }, *saleInventory);
                }, "いいえ", [](Ref *ref) {});
                this->addChild(dialogLayer, ZOrders::Dialog);
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_STOCK, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                // 倉庫に入れる。逆に倉庫側だと持ち物へ。一杯の時はボタン押せなくしたい...
                long objectId = itemDto.getObjectId();
                auto changeInventory = ItemInventoryDto::changeInventory(objectId, &this->_itemInventory, &this->_itemInventoryStock);
                if (changeInventory) {
                    itemWindow->refresh(std::list<ItemInventoryDto>{
                        this->_itemInventory,
                        this->_itemInventoryStock
                    }, *changeInventory);
                } else {
                    // 交換がない
                    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("交換できません。", "とじる", [](Ref *ref) {});
                    this->addChild(dialogLayer, ZOrders::Dialog);
                }
            }
        },
    });
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->setCloseCallback([this]() {
        AccountData::getInstance()->saveInventory(this->_itemInventory, this->_itemInventoryStock);
    });
    this->addChild(itemWindow, ZOrders::ItemInventory);
}

void MypageScene::showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback)
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    auto itemWindow = ItemInventoryLayer::create(this->_itemInventory);
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_MIXED, ItemInventoryLayer::CloseType::CLOSE,
            [this, selectItemCallback](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                CCLOG("itemName = %s", itemDto.getName().c_str());
                
                this->_itemInventory.removeItemDto(itemDto.getObjectId());
                selectItemCallback(itemDto);
            }
        }
    });
    itemWindow->setCloseCallback([this]() {
        //
    });
    this->addChild(itemWindow, ZOrders::ItemInventory);
}



bool MypageScene::mixedItem()
{
    Size winSize = Director::getInstance()->getWinSize();
    
    // 選択チェック
    if (this->_baseItemDto.getObjectId() == 0 || this->_materialItemDto.getObjectId() == 0) {
        CCLOG("ベースか素材が選択されていません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("ベースか素材が選択されていません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::Dialog);
        return false;
    }
    
    // 合成可能かチェック
    if (!ItemLogic::isMixedItem(this->_baseItemDto, this->_materialItemDto)) {
        CCLOG("その組み合わせは合成できません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("その組み合わせは合成できません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::Dialog);
        return false;
    }
    // 金額計算して支払う
    int mixedGold = ItemLogic::calcMixedItemGold(this->_baseItemDto, this->_materialItemDto);
    if (this->_itemInventory.getGold() < mixedGold) {
        CCLOG("お金がたりません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("お金がたりません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::Dialog);
        return false;
    }
    
    // 合成!
    auto mixedItemDto = ItemLogic::mixedItem(this->_baseItemDto, this->_materialItemDto);
    
    // ベースと素材のアイテムを削除
    this->_itemInventory.removeItemDto(this->_baseItemDto.getObjectId());
    this->_itemInventory.removeItemDto(this->_materialItemDto.getObjectId());
    
    // 合成結果のアイテムをインベントリに追加してsaveする
    this->_itemInventory.addItemDto(mixedItemDto);
    AccountData::getInstance()->saveInventory(this->_itemInventory);
    CCLOG("合成成功！");
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(mixedItemDto.createItemName() + "合成に成功しました。", "とじる", [](Ref *ref) {});
    this->addChild(dialogLayer, ZOrders::Dialog);
    return true;
}

/////////////////
// DEBUG

void MypageScene::showDebugMenu()
{
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    
    auto modalLayer = ModalLayer::create();
    Size win_size = Director::getInstance()->getWinSize();
    
    auto systemMenuLayer = SystemMenuLayer::create(win_size * 0.5, "その他・システムメニュー");
    systemMenuLayer->setPosition(CommonWindowUtil::createPointCenter(systemMenuLayer, modalLayer));
    systemMenuLayer->setMenuButtonList(std::list<SystemMenuLayer::SystemMenuButtonInfo>{
        SystemMenuLayer::SystemMenuButtonInfo("ぽっと", [this, modalLayer]() {
            
            for (int i = 0; i < 10; i++) {
                MUseItem mUseItem = MUseItemDao::getInstance()->selectById(1);
                int objectId = AccountData::getInstance()->getGameObjectId();
                ItemDto itemDto(objectId, mUseItem, 0);
                this->_itemInventory.addItemDto(itemDto);
                AccountData::getInstance()->gameObjectCountUp();
            }

            AccountData::getInstance()->saveInventory(this->_itemInventory);
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
        SystemMenuLayer::SystemMenuButtonInfo("おかね", [this, modalLayer]() {
            this->_itemInventory.addGold(10000);
            AccountData::getInstance()->saveInventory(this->_itemInventory);
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
        SystemMenuLayer::SystemMenuButtonInfo("未設定", [this, modalLayer]() {
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
        
        SystemMenuLayer::SystemMenuButtonInfo("未設定", [this, modalLayer]() {
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
        SystemMenuLayer::SystemMenuButtonInfo("未設定", [this, modalLayer]() {
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
        SystemMenuLayer::SystemMenuButtonInfo("未設定", [this, modalLayer]() {
            modalLayer->removeAllChildrenWithCleanup(true); modalLayer->setVisible(false);
        }),
    });
    
    modalLayer->addChild(systemMenuLayer);
    this->addChild(modalLayer, ZOrders::Dialog);
}


NS_ROGUE_END

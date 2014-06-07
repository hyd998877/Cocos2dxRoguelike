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

#include "TopPageLayer.h"
#include "ItemStockPageLayer.h"
#include "ItemMixedPageLayer.h"
#include "QuestPageLayer.h"

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
        this->initQuestSave();
    } else {
        this->initMyPage();
    }

    return true;
}

void MypageScene::changePage(Layer* pageLayer, int selectMenuTag)
{
    if (this->_baseLayer) {
        this->_baseLayer->removeAllChildrenWithCleanup(true);
    }
    this->_baseLayer = Layer::create();
    
    this->_baseLayer->addChild(pageLayer, ZOrders::Page);
    {
        auto menu = createGlobalMenu();
        auto item_menu1 = menu->getChildByTag(selectMenuTag);
        item_menu1->setColor(Color3B::GREEN);
        auto black_layer = LayerColor::create(Color4B::BLACK);
        black_layer->setOpacity(192);
        black_layer->setContentSize(Size(menu->getContentSize().width, item_menu1->getContentSize().height + 8));
        pageLayer->addChild(black_layer);
        pageLayer->addChild(menu);
    }
    
    if (this->_baseLayer) {
        this->addChild(this->_baseLayer);
    }
}

void MypageScene::initQuestSave()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    std::string saveTitle = "前回プレイしたデータが残っています。\n再開しますか？";
    std::string saveDetail = AccountData::getInstance()->createQuestSaveDetailText();
    std::string dialogTitle = saveTitle + "\n\n" + saveDetail + "\n\n※いいえを選択すると、\n所持していたアイテムは消えます";
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(dialogTitle, "はい", [](Ref *ref) {
        auto scene = RogueScene::scene(AccountData::getInstance()->getRoguePlayData().getQuestType(), AccountData::getInstance()->getRoguePlayData().getQuestId());
        auto trans = TransitionFadeDown::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    }, "いいえ", [this](Ref *ref) {
        AccountData::getInstance()->resetRoguePlayData();
        this->initMyPage();
    });
    this->addChild(dialogLayer);
}

void MypageScene::initMyPage()
{
    this->changePage(TopPageLayer::create(), GlobalMenuTags::TOP);
}

void MypageScene::initItemStockPage()
{
    this->changePage(ItemStockPageLayer::create(), GlobalMenuTags::STOCK);
}

void MypageScene::initMixedPage()
{
    this->changePage(ItemMixedPageLayer::create(), GlobalMenuTags::MIXED);
}

void MypageScene::initQuestPage()
{
    // 未プレイはチュートリアルADVへ
    if (AccountData::getInstance()->getGamePlayProgress() == AccountData::GamePlayProgress::INIT) {
        Scene* scene = NovelScene::scene(0, 0, [this]() {
            AccountData::getInstance()->updateGamePlayProgress(AccountData::GamePlayProgress::TUTORIAL_PLAY);
            int play_quest_id = 1;
            auto scene = RogueScene::scene(RoguePlayDto::QuestType::TUTORIAL, play_quest_id);
            auto trans = TransitionFadeDown::create(1.0f, scene);
            Director::getInstance()->replaceScene(trans);
        });
        auto trans = TransitionFadeDown::create(1.0f, scene);
        Director::getInstance()->replaceScene(trans);
    } else {
        this->changePage(QuestPageLayer::create(), GlobalMenuTags::QUEST);
    }
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
        this->initQuestPage();
    });
    item_menu2->setTag(GlobalMenuTags::QUEST);
    
    auto item_menu3 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "そ　う　こ"), WAKU_PADDING, [this](Ref *ref) {
        this->initItemStockPage();
    });
    item_menu3->setTag(GlobalMenuTags::STOCK);
    
    auto item_menu4 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "の　べ　る"), WAKU_PADDING, [this](Ref *ref) {
        Scene* scene = NovelScene::scene(99, 0, [this]() {
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
    
//    auto item_menu6 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "て　す　と"), WAKU_PADDING, [this](Ref *ref) {
//        this->showDebugMenu();
//    });
//    item_menu6->setTag(GlobalMenuTags::TEST);
    
    auto menu = Menu::create(item_menu1, item_menu2, item_menu3, item_menu4, item_menu5, NULL);
    // 4間隔で横に並べる
    menu->alignItemsHorizontallyWithPadding(4);
    menu->setPosition(Point(menu->getContentSize().width / 2, item_menu1->getContentSize().height / 2 + WAKU_PADDING.height / 2));
    
    return menu;
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

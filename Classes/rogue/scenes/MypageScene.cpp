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

#include "ItemInventoryLayer.h"
#include "ItemLogic.h"

#include "AccountData.h"

USING_NS_CC;

NS_ROGUE_BEGIN

MypageScene::MypageScene()
: _itemInventory()
, _itemInventoryStock()
, _baseLayer(nullptr)
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
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(win_size*0.6, dialogTitle, "はい", "いいえ", [](Ref *ref) {
        auto scene = RogueScene::scene(AccountData::getInstance()->getRoguePlayData().quest_id);
        auto trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    }, [this](Ref *ref) {
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
    initHeaderTitle("とっぷぺーじ");
    
    // キャラ表示
    {
        auto actor_sprite = Sprite::create("novel/actor4_novel_s_0.png");
        actor_sprite->setPosition(Point(win_size.width * 0.25, actor_sprite->getContentSize().height * 0.25));
        actor_sprite->setFlippedX(true);
        this->_baseLayer->addChild(actor_sprite);
    }
    
    // セリフ表示
    {
        auto comment_layer = Layer::create();
        comment_layer->setContentSize(Size(win_size.width / 2, win_size.height / 3));
        comment_layer->setPosition(Point(win_size.width * 0.7 - comment_layer->getContentSize().width / 2, win_size.height * 0.5 - comment_layer->getContentSize().height / 2));
        comment_layer->setTag(10); // TODO: とりあえず
        this->_baseLayer->addChild(comment_layer);
        
        auto comment_label = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "ここにはお知らせとか表示する予定よ！\nまだ開発中です。");
        comment_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
        comment_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
        comment_label->setTag(100); // TODO: とりあえず
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
    initHeaderTitle("そうこ");
    
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
    
    // タイトル表示
    initHeaderTitle("ごうせい屋");
    
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
            showMixedItemSelectWindow([label, baseSprite](const ItemDto &itemDto) {
                auto itemSprite = Sprite::create(ItemLogic::createItemImageFileName(itemDto.getImageResId()));
                baseSprite->setTexture(itemSprite->getTexture());
                label->setString(itemDto.createItemName());
            });
        });
        baseItemMenuLabel->setPosition(Point(win_size.width*0.3, win_size.height*0.7));
        
        // 素材
        auto materialSprite = cocos2d::Sprite::createWithSpriteFrame(imageSpriteFrame);
        auto materialItemlayer = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, materialSprite, FontUtils::getDefaultFontTTFConfig(), "そざいを選択してください",[this, materialSprite](Ref *ref) {
            auto label = static_cast<Label*>(static_cast<MenuItemLabel*>(ref)->getLabel());
            showMixedItemSelectWindow([label, materialSprite](const ItemDto &itemDto) {
                auto itemSprite = Sprite::create(ItemLogic::createItemImageFileName(itemDto.getImageResId()));
                materialSprite->setTexture(itemSprite->getTexture());
                label->setString(itemDto.createItemName());
            });
        });
        materialItemlayer->setPosition(Point(win_size.width*0.3, win_size.height*0.3));
        
        // 合成ボタン
        // TODO:
        auto mixedMenuItem = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "合成"), Size(8, 4), [](Ref *ref) {
            
        });
        
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
void MypageScene::initHeaderTitle(const std::string titleText)
{
    Size win_size = Director::getInstance()->getWinSize();
    
    // タイトル表示
    auto title_layer = Layer::create();
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
        
        Scene* scene = NovelScene::scene(2, 0, [this]() {
            CCLOG("novel2 end");
            int play_quest_id = 1;
            auto scene = RogueScene::scene(play_quest_id);
            auto trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
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
        Scene* scene = SRPGScene::scene();
        TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
        Director::getInstance()->replaceScene(trans);
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
    this->_itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
    std::list<ItemInventoryDto> itemInventoryList{
        this->_itemInventory,
        this->_itemInventoryStock
    };
    
    auto itemWindow = ItemInventoryLayer::create(itemInventoryList);
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_DROP, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                
                // TODO: 本当に捨てていいですか的なやつ
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_SALE, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                
                // TODO: 本当に売っていいですか的なやつ
                // TODO: アイテム値段持ってねーよ・・・マスタぁああああ ItemLogicでsale()を追加かな
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
                    // TODO: エラーもここに？
                }
            }
        },
    });
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->setCloseCallback([this]() {
        AccountData::getInstance()->saveInventory(this->_itemInventory, this->_itemInventoryStock);
    });
    this->addChild(itemWindow, 99999);
}

void MypageScene::showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback)
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    std::list<ItemInventoryDto> itemInventoryList{
        this->_itemInventory
    };
    
    auto itemWindow = ItemInventoryLayer::create(itemInventoryList);
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_MIXED, ItemInventoryLayer::CloseType::CLOSE,
            [this, selectItemCallback](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                CCLOG("itemName = %s", itemDto.getName().c_str());
                selectItemCallback(itemDto);
            }
        }
    });
    itemWindow->setCloseCallback([this]() {
        //AccountData::getInstance()->saveInventory(this->_itemInventory, this->_itemInventoryStock);
    });
    this->addChild(itemWindow, 99999);
}

NS_ROGUE_END

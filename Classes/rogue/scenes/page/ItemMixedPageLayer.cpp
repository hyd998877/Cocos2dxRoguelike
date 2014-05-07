/**
 @file ItemMixedPageLayer.cpp
 
 @brief アイテム合成画面
 
 アイテムの合成屋
 
 @author kyokomi
 @date 2014/05/07
 
 */

#include "ItemMixedPageLayer.h"

#include "AccountData.h"

#include "CommonWindowUtil.h"
#include "AlertDialogLayer.h"
#include "ItemInventoryLayer.h"

#include "ItemLogic.h"

#include "MypageHeaderLayer.h"

USING_NS_CC;

ItemMixedPageLayer::ItemMixedPageLayer()
{
    
}

ItemMixedPageLayer::~ItemMixedPageLayer()
{
    
}

bool ItemMixedPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    this->loadInventory();

    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/shop03_a.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width / background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    // キャラ表示
    auto playerSprite = Sprite::create("novel/actor10_novel_s_1.png");
    playerSprite->setScale(resolutionSize.height/playerSprite->getContentSize().height);
    playerSprite->setPosition(Point(winSize.width * 0.85, winSize.height * 0.4));
    this->addChild(playerSprite);
    
    Size layerSize = Size(winSize.width*0.5, winSize.height*0.3);
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
        this->showMixedItemSelectWindow([this, label, baseSprite](const ItemDto &itemDto) {
            this->refreshSelectItem(baseSprite, label, itemDto);
            this->setBaseItem(itemDto);
        });
    });
    baseItemMenuLabel->setPosition(Point(winSize.width*0.275, winSize.height*0.7));
    
    // 素材
    auto materialSprite = cocos2d::Sprite::createWithSpriteFrame(imageSpriteFrame);
    auto materialItemlayer = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, materialSprite, FontUtils::getDefaultFontTTFConfig(), "そざいを選択してください",[this, materialSprite](Ref *ref) {
        auto label = static_cast<Label*>(static_cast<MenuItemLabel*>(ref)->getLabel());
        this->showMixedItemSelectWindow([this, label, materialSprite](const ItemDto &itemDto) {
            this->refreshSelectItem(materialSprite, label, itemDto);
            this->setMaterialItem(itemDto);
        });
    });
    materialItemlayer->setPosition(Point(winSize.width*0.275, winSize.height*0.3));
    
    // L表示
    auto sprite1 = Sprite::create("ui/l_image.png");
    sprite1->setPosition(Point(winSize.width*0.575, winSize.height*0.65));
    this->addChild(sprite1);
    auto sprite2 = Sprite::create("ui/l_image.png");
    sprite2->setFlippedY(true);
    sprite2->setPosition(Point(winSize.width*0.575, winSize.height*0.35));
    this->addChild(sprite2);
    
    // 合成ボタン
    auto mixedMenuItem = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getStrongFontTTFConfig(), "合成"), Size(16, 8), [this, baseSprite, baseItemMenuLabel, materialSprite, materialItemlayer](Ref *ref) {
        // 合成実行
        if (this->mixedItem()) {
            // セーブ
            this->saveInventory();
            
            // 成功したらベースと素材を未選択にする
            auto emptyItemDto = ItemDto();
            auto baseLabel = static_cast<Label*>(static_cast<MenuItemLabel*>(baseItemMenuLabel)->getLabel());
            this->refreshSelectItem(baseSprite, baseLabel, emptyItemDto);
            this->setBaseItem(emptyItemDto);
            auto materialLabel = static_cast<Label*>(static_cast<MenuItemLabel*>(materialItemlayer)->getLabel());
            this->refreshSelectItem(materialSprite, materialLabel, emptyItemDto);
            this->setMaterialItem(emptyItemDto);
        }
    });
    mixedMenuItem->setPosition(Point(winSize.width*0.675, winSize.height*0.5));
    
    auto menu = Menu::create(baseItemMenuLabel, materialItemlayer, mixedMenuItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
    
    auto headerLayer = MypageHeaderLayer::create();
    headerLayer->setTitleText("合成屋");
    this->addChild(headerLayer);
    
    return true;
}

bool ItemMixedPageLayer::mixedItem()
{
    // 選択チェック
    if (this->_baseItemDto.getObjectId() == 0 || this->_materialItemDto.getObjectId() == 0) {
        CCLOG("ベースか素材が選択されていません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("ベースか素材が選択されていません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::DIALOG);
        return false;
    }
    
    // 合成可能かチェック
    if (!ItemLogic::isMixedItem(this->_baseItemDto, this->_materialItemDto)) {
        CCLOG("その組み合わせは合成できません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("その組み合わせは合成できません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::DIALOG);
        return false;
    }
    // 金額計算して支払う
    int mixedGold = ItemLogic::calcMixedItemGold(this->_baseItemDto, this->_materialItemDto);
    if (this->_itemInventory.getGold() < mixedGold) {
        CCLOG("お金がたりません");
        auto dialogLayer = AlertDialogLayer::createWithContentSizeModal("お金がたりません", "とじる", [](Ref *ref) {});
        this->addChild(dialogLayer, ZOrders::DIALOG);
        return false;
    }
    
    // 合成!
    auto mixedItemDto = ItemLogic::mixedItem(this->_baseItemDto, this->_materialItemDto);
    
    // ベースと素材のアイテムを削除
    this->_itemInventory.removeItemDto(this->_baseItemDto.getObjectId());
    this->_itemInventory.removeItemDto(this->_materialItemDto.getObjectId());
    
    // 合成結果のアイテムをインベントリに追加してsaveする
    this->_itemInventory.addItemDto(mixedItemDto);
    CCLOG("合成成功！");
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(mixedItemDto.createItemName() + "合成に成功しました。", "とじる", [](Ref *ref) {});
    this->addChild(dialogLayer, ZOrders::DIALOG);
    return true;
}

void ItemMixedPageLayer::showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback)
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
    this->addChild(itemWindow, ZOrders::DIALOG);
}


void ItemMixedPageLayer::loadInventory()
{
    this->_itemInventory = RogueLikeGame::AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
}

void ItemMixedPageLayer::saveInventory()
{
    RogueLikeGame::AccountData::getInstance()->saveInventory(this->_itemInventory);
}

void ItemMixedPageLayer::setBaseItem(const ItemDto &itemDto)
{
    this->_baseItemDto = itemDto;
}
void ItemMixedPageLayer::setMaterialItem(const ItemDto &itemDto)
{
    this->_materialItemDto = itemDto;
}
void ItemMixedPageLayer::refreshSelectItem(Sprite* materialSprite, Label* label, const ItemDto &itemDto)
{
    if (itemDto.getObjectId() != 0) {
        auto itemSprite = Sprite::create(ItemLogic::createItemImageFileName(itemDto.getImageResId()));
        materialSprite->setTexture(itemSprite->getTexture());
        label->setString(itemDto.createItemName());
    } else {
        std::string spriteFrameFileName = "grid32.png";
        auto imageSpriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameFileName);
        if (!imageSpriteFrame) {
            imageSpriteFrame = cocos2d::Sprite::create(spriteFrameFileName)->getSpriteFrame();
            cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(imageSpriteFrame, spriteFrameFileName);
        }
        materialSprite->setTexture(imageSpriteFrame->getTexture());
        label->setString("");
    }
}


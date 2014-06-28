/**
 @file ItemMixedPageLayer.cpp
 
 @brief アイテム合成画面
 
 アイテムの合成屋
 
 @author kyokomi
 @date 2014/05/07
 
 */

#include "ItemMixedPageLayer.h"

#include "AppMacros.h"
#include "CommonWindowUtil.h"

#include "ItemDto.h"
#include "ItemLogic.h"

USING_NS_CC;

ItemMixedPageLayer::ItemMixedPageLayer()
: _menuCallback1(nullptr)
, _menuCallback2(nullptr)
, _menuCallback3(nullptr)
, _baseSprite(nullptr)
, _materialSprite(nullptr)
, _baseMenuItemLabel(nullptr)
, _materialMenuItemLabel(nullptr)
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
        if (_menuCallback1) {
            _menuCallback1(ref);
        }
    });
    baseItemMenuLabel->setPosition(Point(winSize.width*0.275, winSize.height*0.7));
    _baseSprite = baseSprite;
    _baseMenuItemLabel = baseItemMenuLabel;
    
    // 素材
    auto materialSprite = cocos2d::Sprite::createWithSpriteFrame(imageSpriteFrame);
    auto materialItemlayer = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, materialSprite, FontUtils::getDefaultFontTTFConfig(), "そざいを選択してください",[this, materialSprite](Ref *ref) {
        if (_menuCallback2) {
            _menuCallback2(ref);
        }
    });
    materialItemlayer->setPosition(Point(winSize.width*0.275, winSize.height*0.3));
    _materialSprite = materialSprite;
    _materialMenuItemLabel = materialItemlayer;
    
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
        if (_menuCallback3) {
            _menuCallback3(ref);
        }
    });
    mixedMenuItem->setPosition(Point(winSize.width*0.675, winSize.height*0.5));
    
    auto menu = Menu::create(baseItemMenuLabel, materialItemlayer, mixedMenuItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
    
    return true;
}

void ItemMixedPageLayer::refreshSelectBase(const ItemDto &itemDto)
{
    refreshSelectItem(_baseSprite, _baseMenuItemLabel, itemDto);
}

void ItemMixedPageLayer::refreshSelectMaterial(const ItemDto &itemDto)
{
    refreshSelectItem(_materialSprite, _materialMenuItemLabel, itemDto);
}

void ItemMixedPageLayer::refreshSelectItem(Sprite* materialSprite, MenuItem* menuItem, const ItemDto &itemDto)
{
    auto label = static_cast<Label*>(static_cast<MenuItemLabel*>(menuItem)->getLabel());
    
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

void ItemMixedPageLayer::mixedItemAfter()
{
    // 成功したらベースと素材を未選択にする
    auto emptyItemDto = ItemDto();
    this->refreshSelectItem(_baseSprite, _baseMenuItemLabel, emptyItemDto);
    this->refreshSelectItem(_materialSprite, _materialMenuItemLabel, emptyItemDto);
}



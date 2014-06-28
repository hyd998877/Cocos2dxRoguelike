/**
 @file ItemStockPageLayer.cpp
 
 @brief 倉庫画面
 
 倉庫のアイテムと所持アイテムの交換。
 アイテムを捨てたり売ったり整理できます。
 
 @author kyokomi
 @date 2014/05/07
 
 */

#include "ItemStockPageLayer.h"

#include "AppMacros.h"
#include "CommonWindowUtil.h"

USING_NS_CC;

ItemStockPageLayer::ItemStockPageLayer()
: _menuCallback1(nullptr)
, _menuCallback2(nullptr)
{
}

ItemStockPageLayer::~ItemStockPageLayer()
{
}

bool ItemStockPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/ci_shopbakery01a_b.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width/background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    // キャラ表示
    auto playerSprite = Sprite::create("novel/actor11_novel_s_0.png");
    playerSprite->setScale(resolutionSize.height/playerSprite->getContentSize().height);
    playerSprite->setPosition(Point(winSize.width * 0.8, winSize.height * 0.4));
    this->addChild(playerSprite);
    
    // メッセージ
    auto messageLayer = LayerColor::create(Color4B::BLACK);
    messageLayer->setContentSize(Size(winSize.width / 3.5, winSize.height / 4.0));
    messageLayer->setPosition(Point(winSize.width * 0.55 - messageLayer->getContentSize().width / 2,
                                    winSize.height * 0.7 - messageLayer->getContentSize().height / 2));
    auto messageTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "大事なものは\n預けておけば安心！");
    messageTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    messageTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    messageLayer->addChild(messageTextLabel);
    messageTextLabel->setPosition(Point(messageLayer->getContentSize().width / 2, messageLayer->getContentSize().height / 2));
    // メッセージ枠
    CommonWindowUtil::attachWindowWaku(messageLayer);
    this->addChild(messageLayer);
    
    Size layerSize = Size(winSize.width*0.35, winSize.height*0.3);
    
    auto iconNode1 = Sprite::create("icon_set/item_696.png");
    auto menuItem1 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode1, FontUtils::getDefaultFontTTFConfig(), "倉庫を開く", [this](Ref *ref) {
        this->_menuCallback1(ref);
    });
    auto iconNode2 = Sprite::create("icon_set/item_671.png");
    auto menuItem2 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode2, FontUtils::getDefaultFontTTFConfig(), "銀行を開く", [this](Ref *ref) {
        this->_menuCallback2(ref);
    });
    
    auto menu = Menu::create(menuItem1, menuItem2, NULL);
    menu->setPosition(Point(winSize.width/5, winSize.height/2));
    menu->alignItemsVerticallyWithPadding(10);
    this->addChild(menu);

    return true;
}


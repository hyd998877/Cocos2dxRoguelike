//
//  QuestPageLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/07.
//
//

#include "QuestPageLayer.h"

#include "AccountData.h"

#include "CommonWindowUtil.h"
//#include "AlertDialogLayer.h"

#include "RogueScene.h"

#include "MRogueMap.h"
#include "MDungeonDao.h"

USING_NS_CC;

NS_ROGUE_BEGIN

QuestPageLayer::QuestPageLayer()
: _menuCallback1(nullptr)
, _menuCallback2(nullptr)
, _menuCallback3(nullptr)
{
    
}

QuestPageLayer::~QuestPageLayer()
{
    
}

bool QuestPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/ad_forest01_c.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width/background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    Size layerSize = Size(winSize.width*0.35, winSize.height*0.2);
    
    auto iconNode1 = Sprite::create("icon_set/item_0.png");
    auto menuItem1 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode1, FontUtils::getDefaultFontTTFConfig(), createQuestName(RoguePlayDto::QuestType::TUTORIAL), [this](Ref *ref) {
        if (_menuCallback1) {
            _menuCallback1(ref);
        }
    });

    auto iconNode2 = Sprite::create("icon_set/item_0.png");
    auto menuItem2 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode2, FontUtils::getDefaultFontTTFConfig(), createQuestName(RoguePlayDto::QuestType::MAIN_QUEST), [this](Ref *ref) {
        if (_menuCallback2) {
            _menuCallback2(ref);
        }
    });
        
    auto iconNode3 = Sprite::create("icon_set/item_0.png");
    auto menuItem3 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode3, FontUtils::getDefaultFontTTFConfig(), createQuestName(RoguePlayDto::QuestType::DEEP_QUEST), [this](Ref *ref) {
        if (_menuCallback3) {
            _menuCallback3(ref);
        }
    });
    
    auto menu = Menu::create(menuItem1, menuItem2, menuItem3, NULL);
    menu->setPosition(Point(winSize.width/5, winSize.height/2));
    menu->alignItemsVerticallyWithPadding(5);
    this->addChild(menu);
    
    return true;
}

std::string QuestPageLayer::createQuestName(const RoguePlayDto::QuestType& questType)
{
    auto questKey = RoguePlayDto::findQuestKey(questType);
    auto dungeon = MDungeonDao::getInstance()->selectById(questKey);
    
    return dungeon.dungeonName;
}

NS_ROGUE_END



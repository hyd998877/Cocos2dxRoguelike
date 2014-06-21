/**

@file  MypageFotterLayer.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#include "MypageFotterLayer.h"
#include "WidgetUtil.h"

#include "MypageScene.h"
#include "RogueScene.h"
#include "BankScene.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;

MypageFotterLayer::MypageFotterLayer()
{
    
}

MypageFotterLayer::~MypageFotterLayer()
{
    
}

bool MypageFotterLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = GUIReader::getInstance()->widgetFromJsonFile("cocostudio/mypage_fotter.json");
    layout->setScale(0.75f);
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(), 0));
    this->addChild(layout);
    
    //攻撃ボタンイベントの登録
    WidgetUtil::onTouchEventEnded(layout, "fotter_button_0", [](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        CCLOG("onTouchEventEnded 0");
        Director::getInstance()->replaceScene(RogueLikeGame::MypageScene::scene());
    });
    WidgetUtil::onTouchEventEnded(layout, "fotter_button_1", [](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        CCLOG("onTouchEventEnded 1");
        Director::getInstance()->replaceScene(RogueLikeGame::MypageScene::scene());
    });
    WidgetUtil::onTouchEventEnded(layout, "fotter_button_2", [](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        CCLOG("onTouchEventEnded 2");
        Director::getInstance()->replaceScene(BankScene::scene());
    });
    WidgetUtil::onTouchEventEnded(layout, "fotter_button_3", [](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        CCLOG("onTouchEventEnded 3");
        Director::getInstance()->replaceScene(RogueLikeGame::MypageScene::scene());
    });
    WidgetUtil::onTouchEventEnded(layout, "fotter_button_4", [](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        CCLOG("onTouchEventEnded 4");
        Director::getInstance()->replaceScene(RogueLikeGame::MypageScene::scene());
    });
    _baseLayout = layout;
    
    return true;
}

Size MypageFotterLayer::getBaseLayoutSize()
{
    return _baseLayout->getContentSize();
}

void MypageFotterLayer::setLockMenu(int menuId)
{
    auto button = dynamic_cast<Button*>(WidgetUtil::getChildByNameRecursion(_baseLayout, cocos2d::StringUtils::format("fotter_button_%d", menuId)));
    button->setTitleColor(Color3B::GREEN);
    button->setColor(Color3B::GREEN);
    button->setEnabled(false);
}

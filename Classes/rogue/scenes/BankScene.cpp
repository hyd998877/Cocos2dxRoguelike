/**

@file  BankScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#include "BankScene.h"

#include "WidgetUtil.h"
#include "MypageFotterLayer.h"
#include "MypageHeaderLayer.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;

BankScene::BankScene()
{
    
}

BankScene::~BankScene()
{
    
}

Scene* BankScene::scene() {
    Scene *scene = Scene::create();
    auto layer = BankScene::create();
    scene->addChild(layer);
    return scene;
}

bool BankScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto bankUiLayout = GUIReader::getInstance()->widgetFromJsonFile("cocostudio/bank_ui2/bank_ui.json");
    bankUiLayout->setScale(0.75f);
    bankUiLayout->setPosition(Vec2(visibleSize.width/2 - bankUiLayout->getContentSize().width/2*bankUiLayout->getScaleX(),
                                   visibleSize.height/2 - bankUiLayout->getContentSize().height/2*bankUiLayout->getScaleY()));
    this->addChild(bankUiLayout);

    // Header
    auto header = MypageHeaderLayer::create();
    header->setTitleText("銀行屋");
    this->addChild(header);
    
    // Fotter
    auto fotter = MypageFotterLayer::create();
    fotter->setLockMenu(2);
    this->addChild(fotter);
    
    return true;
}


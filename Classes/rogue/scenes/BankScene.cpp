/**

@file  BankScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#include "BankScene.h"

#include "cocostudio/CocoStudio.h"

USING_NS_CC;

using namespace cocostudio;

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
    
    return true;
}


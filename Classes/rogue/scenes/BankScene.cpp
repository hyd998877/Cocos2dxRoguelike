/**

@file  BankScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#include "BankScene.h"

USING_NS_CC;

BankScene::BankScene()
{
    
}

BankScene::~BankScene()
{
    
}

Node* BankScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = createCocoStudioWidget("cocostudio/bank_ui2/bank_ui.json");
    layout->setScale(0.75f);
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));
    return layout;
}


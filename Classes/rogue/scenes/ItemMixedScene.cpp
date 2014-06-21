/**

@file  ItemMixedScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#include "ItemMixedScene.h"
#include "ItemMixedPageLayer.h"

USING_NS_CC;

ItemMixedScene::ItemMixedScene()
{
    
}

ItemMixedScene::~ItemMixedScene()
{
    
}

Node* ItemMixedScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = ItemMixedPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));
    return layout;
}

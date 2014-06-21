/**

@file  ItemStockScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#include "ItemStockScene.h"
#include "ItemStockPageLayer.h"

USING_NS_CC;

ItemStockScene::ItemStockScene()
{
    
}

ItemStockScene::~ItemStockScene()
{
    
}

Node* ItemStockScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = ItemStockPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));
    return layout;
}


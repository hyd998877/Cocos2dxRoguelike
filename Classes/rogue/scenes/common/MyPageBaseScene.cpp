/**

@file  MyPageBaseScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#include "MyPageBaseScene.h"
#include "WidgetUtil.h"
#include "MypageFotterLayer.h"
#include "MypageHeaderLayer.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;

MyPageBaseScene::MyPageBaseScene()
{
    
}

MyPageBaseScene::~MyPageBaseScene()
{
    
}

bool MyPageBaseScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // dialog
    auto dialog = Layer::create();
    this->addChild(dialog, ZOrder::DIALOG);
    _dialogLayer = dialog;
    
    // body
    auto layout = initLayout();
    this->addChild(layout, ZOrder::BODY);

    // Header
    auto header = MypageHeaderLayer::create();
    header->setTitleText(getTitleName());
    this->addChild(header, ZOrder::HEADER);
    
    // Fotter
    auto fotter = MypageFotterLayer::create();
    fotter->setLockMenu(getMenuId());
    this->addChild(fotter, ZOrder::FOTTER);
    
    return true;
}

void MyPageBaseScene::addChildDialog(Node* node)
{
    _dialogLayer->addChild(node);
}


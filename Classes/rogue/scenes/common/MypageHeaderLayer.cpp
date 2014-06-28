/**

@file  MypageHeaderLayer.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#include "MypageHeaderLayer.h"
#include "WidgetUtil.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;

MypageHeaderLayer::MypageHeaderLayer()
{
    
}

MypageHeaderLayer::~MypageHeaderLayer()
{
    
}

bool MypageHeaderLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = GUIReader::getInstance()->widgetFromJsonFile("cocostudio/mypage_header.json");
    layout->setScale(0.75f);
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height - layout->getContentSize().height*layout->getScaleY()));
    this->addChild(layout);
    
    _baseLayout = layout;
    
    return true;
}


Size MypageHeaderLayer::getBaseLayoutSize()
{
    return _baseLayout->getContentSize();
}

void MypageHeaderLayer::setTitleText(const std::string& titleName)
{
    auto label = dynamic_cast<Text*>(WidgetUtil::getChildByNameRecursion(_baseLayout, "header_title_label"));
    label->setString(titleName);
}
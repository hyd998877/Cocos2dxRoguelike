//
//  MypageHeaderLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/07.
//
//

#include "MypageHeaderLayer.h"
#include "AppMacros.h"

#include "CommonWindowUtil.h"

USING_NS_CC;

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
    
    Size winSize = Director::getInstance()->getWinSize();

    // タイトル表示
    auto title_layer = LayerColor::create(Color4B::BLACK);
    title_layer->setContentSize(Size(winSize.width, winSize.height / 8));
    title_layer->setPosition(Point(0, winSize.height - title_layer->getContentSize().height));
    this->addChild(title_layer);
    
    auto titleTextlabel = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), " ");
    titleTextlabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    titleTextlabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    titleTextlabel->setPosition(Point(title_layer->getContentSize().width /2, title_layer->getContentSize().height / 2));
    title_layer->addChild(titleTextlabel);
    this->_titleTextLabel = titleTextlabel;
    
    // 枠
    auto window_waku = CommonWindowUtil::createWindowWaku(Size(winSize.width, titleTextlabel->getContentSize().height + titleTextlabel->getSystemFontSize()));
    title_layer->addChild(window_waku);
    
    return true;
}

void MypageHeaderLayer::setTitleText(const std::string &titleText)
{
    this->_titleTextLabel->setString(titleText);
}


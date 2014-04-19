//
//  SystemMenuLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/19.
//
//

#include "SystemMenuLayer.h"
#include "AppMacros.h"
#include "CommonWindowUtil.h"

USING_NS_CC;

SystemMenuLayer::SystemMenuLayer()
{
}

SystemMenuLayer::~SystemMenuLayer()
{
}

SystemMenuLayer* SystemMenuLayer::create(const Size& contentSize, const std::string& titleText, const std::list<SystemMenuButtonInfo> menuButtonInfoList, const cocos2d::Color3B& color /* = cocos2d::Color3B::BLACK */, const GLubyte& opacity /* = 192 */)
{
    SystemMenuLayer *pRet = new SystemMenuLayer();
    if (pRet && pRet->init(contentSize, titleText, menuButtonInfoList, color, opacity)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool SystemMenuLayer::init(const Size& contentSize, const std::string& titleText, const std::list<SystemMenuButtonInfo> menuButtonInfoList, const Color3B& color, const GLubyte& opacity)
{
    if (!LayerColor::init()) {
        return false;
    }
    
    this->setColor(Color3B::RED);
    this->setOpacity(opacity);
    this->setContentSize(contentSize);
    
    // TitleLabel
    initCreateTitleLabel(titleText);
    
    // Menu
    initCreateMenuList(menuButtonInfoList);
    
    return true;
}

void SystemMenuLayer::initCreateTitleLabel(const std::string &titleText)
{
    auto titleLabel = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), titleText);
    titleLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    // 文字はcenterにしているので中心におけばOKなので0を指定
    titleLabel->setPosition(CommonWindowUtil::createPointCenterXTopY(Size(0, titleLabel->getContentSize().height), this->getContentSize()));
    this->addChild(titleLabel);
}

void SystemMenuLayer::initCreateMenuList(const std::list<SystemMenuButtonInfo> menuButtonInfoList)
{
    
}


//
//  AlertDialogLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//
#include "AppMacros.h"

#include "AlertDialogLayer.h"

USING_NS_CC;

AlertDialogLayer::~AlertDialogLayer()
{
    
}

AlertDialogLayer* AlertDialogLayer::createWithContentSize(Size contentSize, std::string titleText, std::string okText, std::string ngText)
{
    AlertDialogLayer *pRet = new AlertDialogLayer();
    if (pRet && pRet->initWithContentSize(contentSize, titleText, okText, ngText))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool AlertDialogLayer::initWithContentSize(Size contentSize, std::string titleText, std::string okText, std::string ngText)
{
    if (!LayerColor::initWithColor(Color4B::BLACK))
    {
        return false;
    }
    setContentSize(contentSize);
    CommonWindowUtil::attachWindowWaku(this);
    
    // タイトル内容
    const int font_size = 20;
    auto pTitle = Label::create(titleText, GAME_FONT(font_size), GAME_FONT_SIZE(font_size));
    pTitle->setTag(AlertDialogLayer::TitleTextTag);
    this->addChild(pTitle);
    
    setTitleText(titleText);
    
    // --------------
    // ボタン生成
    const Size WAKU_PADDING = Size(8, 4);
    
    // OKボタン
    auto pOkMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::create(okText, GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), WAKU_PADDING, [this](Ref *pSender) {
        if (m_okMenuItemCallback)
        {
            this->m_okMenuItemCallback(pSender);
        }
    });
    pOkMenuItemLabel->setPosition(Point(this->getContentSize().width * 0.25, pOkMenuItemLabel->getContentSize().height));
    pOkMenuItemLabel->setTag(AlertDialogLayer::OK_MenuItemLabelTag);
    
    // NGボタン
    auto pNgMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::create(ngText, GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), WAKU_PADDING, [this](Ref *pSender) {
        if (m_ngMenuItemCallback)
        {
            this->m_ngMenuItemCallback(pSender);
        }
    });
    pNgMenuItemLabel->setPosition(Point(this->getContentSize().width * 0.75, pNgMenuItemLabel->getContentSize().height));
    pNgMenuItemLabel->setTag(AlertDialogLayer::NG_MenuItemLabelTag);
    
    auto pCommonWindowMenu = Menu::create(pOkMenuItemLabel, pNgMenuItemLabel, NULL);
    pCommonWindowMenu->setPosition(Point::ZERO);
    pCommonWindowMenu->setTag(AlertDialogLayer::MenuTag);
    this->addChild(pCommonWindowMenu);
    
    return true;
}

void AlertDialogLayer::setTitleText(const std::string& text)
{
    auto pLabel = dynamic_cast<Label*>(getChildByTag(AlertDialogLayer::TitleTextTag));
    if (pLabel)
    {
        pLabel->setString(text);
        pLabel->setPosition(Point(this->getContentSize().width * 0.5, this->getContentSize().height * 0.75));
    }
}

void AlertDialogLayer::setOkText(const std::string& text)
{
    auto pLabel = dynamic_cast<MenuItemLabel*>(getChildByTag(AlertDialogLayer::MenuTag)->getChildByTag(AlertDialogLayer::OK_MenuItemLabelTag));
    if (pLabel)
    {
        pLabel->setString(text);
        pLabel->setPosition(Point(this->getContentSize().width * 0.25, pLabel->getContentSize().height));
    }
}

void AlertDialogLayer::setNgText(const std::string& text)
{
    auto pLabel = dynamic_cast<MenuItemLabel*>(getChildByTag(AlertDialogLayer::MenuTag)->getChildByTag(AlertDialogLayer::NG_MenuItemLabelTag));
    if (pLabel)
    {
        pLabel->setString(text);
        pLabel->setPosition(Point(this->getContentSize().width * 0.75, pLabel->getContentSize().height));
    }
}




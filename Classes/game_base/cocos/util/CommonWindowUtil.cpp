//
//  CommonWindowUtil.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//

#include "CommonWindowUtil.h"

USING_NS_CC;

#pragma mark
#pragma mark 汎用

cocos2d::Point CommonWindowUtil::createPointCenter(cocos2d::Node* baseNode, cocos2d::Node* targetNode) {
    return createPointCenter(baseNode->getContentSize(), targetNode->getContentSize());
}
cocos2d::Point CommonWindowUtil::createPointCenter(const cocos2d::Size& baseNodeSize, const cocos2d::Size& targetNodeSize) {
    return cocos2d::Point(targetNodeSize.width / 2 - baseNodeSize.width / 2,
                          targetNodeSize.height / 2 - baseNodeSize.height / 2);
}

cocos2d::Point CommonWindowUtil::createPointCenterXTopY(const cocos2d::Size& baseNodeSize, const cocos2d::Size& targetNodeSize) {
    return cocos2d::Point(targetNodeSize.width / 2 - baseNodeSize.width / 2,
                          targetNodeSize.height- baseNodeSize.height);
}

#pragma mark
#pragma mark メニュー

cocos2d::MenuItemLabel* CommonWindowUtil::createMenuItemLabelWaku(cocos2d::Label* pLabel, cocos2d::Size wakuPadding, const cocos2d::ccMenuCallback& callback)
{
    if (pLabel)
    {
        // ラベルメニュー作成
        auto pMenuItemLabel = cocos2d::MenuItemLabel::create(pLabel, callback);
        
        // 枠作成
        auto pMenuItemLabelWaku = CommonWindowUtil::createWindowWaku(pMenuItemLabel, wakuPadding);
        
        // 枠のサイズにボタンを広げる
        if (wakuPadding.width != 0 || wakuPadding.height != 0)
        {
            pMenuItemLabel->setContentSize(pMenuItemLabelWaku->getContentSize());
            // ラベルの位置をずらす
            pLabel->setPosition(cocos2d::Point(pMenuItemLabel->getContentSize().width / 2 - pLabel->getContentSize().width / 2, pMenuItemLabel->getContentSize().height / 2 - pLabel->getContentSize().height / 2));
        }
        
        pMenuItemLabel->addChild(pMenuItemLabelWaku);
        
        return pMenuItemLabel;
    }
    else
    {
        return nullptr;
    }
}

#pragma mark
#pragma mark 共通枠

void CommonWindowUtil::attachWindowWaku(cocos2d::Node* pNode)
{
    auto pWaku = createWindowWaku(pNode);
    pNode->addChild(pWaku);
}
void CommonWindowUtil::attachWindowWaku(cocos2d::Node* pNode, cocos2d::Size padding)
{
    auto pWaku = createWindowWaku(pNode, padding);
    pNode->addChild(pWaku);
}

cocos2d::extension::Scale9Sprite* CommonWindowUtil::createWindowWaku()
{
    return cocos2d::extension::Scale9Sprite::create("ui/dot_waku.png", cocos2d::Rect(0, 0, 16, 16), cocos2d::Rect(3, 3, 10, 10));
}

cocos2d::extension::Scale9Sprite* CommonWindowUtil::createWindowWaku(cocos2d::Node* pNode)
{
    return createWindowWaku(pNode, cocos2d::Size::ZERO);
}

cocos2d::extension::Scale9Sprite* CommonWindowUtil::createWindowWaku(cocos2d::Node* pNode, cocos2d::Size padding)
{
    return createWindowWaku(pNode->getContentSize() + padding * 2);
}

cocos2d::extension::Scale9Sprite* CommonWindowUtil::createWindowWaku(cocos2d::Size contentSize)
{
    auto pWaku = createWindowWaku();
    pWaku->setPreferredSize(contentSize);
    pWaku->setPosition(pWaku->getPreferredSize().width / 2, pWaku->getPreferredSize().height / 2);
    return pWaku;
}

#pragma mark
#pragma mark sprite

cocos2d::MenuItem* CommonWindowUtil::createMenuItemLabelWithSpriteIcon(const cocos2d::Size& layerSize,
                                                                       cocos2d::Node* iconNode,
                                                                       const cocos2d::TTFConfig& ttfConfig,
                                                                       const std::string& text,
                                                                       const cocos2d::ccMenuCallback &callback)
{
    auto textLabel = cocos2d::Label::createWithTTF(ttfConfig, text);
    // ラベルメニュー作成
    auto baseMenuItemLabel = cocos2d::MenuItemLabel::create(textLabel, callback);
    baseMenuItemLabel->setContentSize(layerSize);
    
    iconNode->setPosition(cocos2d::Point(iconNode->getContentSize().width,
                                       baseMenuItemLabel->getContentSize().height /2));
    baseMenuItemLabel->addChild(iconNode);
    
    // 枠作成
    auto pMenuItemLabelWaku = CommonWindowUtil::createWindowWaku(baseMenuItemLabel);
    baseMenuItemLabel->addChild(pMenuItemLabelWaku);

    textLabel->setPosition(cocos2d::Point(iconNode->getPositionX() + baseMenuItemLabel->getContentSize().width / 2 - textLabel->getContentSize().width / 2,
                                          baseMenuItemLabel->getContentSize().height / 2 - textLabel->getContentSize().height / 2));
    
    return baseMenuItemLabel;
}




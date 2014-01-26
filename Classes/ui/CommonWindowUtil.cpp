//
//  CommonWindowUtil.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//

#include "CommonWindowUtil.h"


cocos2d::MenuItemLabel* CommonWindowUtil::createMenuItemLabelWaku(cocos2d::LabelTTF* pLabel, cocos2d::Size wakuPadding, const cocos2d::ccMenuCallback& callback)
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

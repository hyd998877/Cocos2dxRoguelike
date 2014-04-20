//
//  CommonWindowUtil.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//

#include "CommonWindowUtil.h"

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

// TODO: (kyokomi) ちゃんと専用クラスつくらないとダメだこりゃ（SpriteとTextの更新でtagはイケてない）
cocos2d::Layer* CommonWindowUtil::createSpriteWithLabelLayer(const cocos2d::Size& layerSize, const std::string& spriteFrameFileName, const cocos2d::TTFConfig& ttfConfig, const std::string& text) {
    auto equipWeaponLayer = cocos2d::LayerColor::create(cocos2d::Color4B::BLACK);
    equipWeaponLayer->setOpacity(128);
    equipWeaponLayer->setContentSize(layerSize);
    CommonWindowUtil::attachWindowWaku(equipWeaponLayer);
    
    auto equipWeaponSprite = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameFileName);
    if (!equipWeaponSprite) {
        equipWeaponSprite = cocos2d::Sprite::create(spriteFrameFileName)->getSpriteFrame();
        cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(equipWeaponSprite, spriteFrameFileName);
    }
    auto sprite = cocos2d::Sprite::createWithSpriteFrame(equipWeaponSprite);
    sprite->setPosition(cocos2d::Point(sprite->getContentSize().width / 2, equipWeaponLayer->getContentSize().height /2));
    sprite->setTag(1);// TODO: (kyokomi)とりあえず...サーセン
    equipWeaponLayer->addChild(sprite);
    
    auto textLabel = cocos2d::Label::createWithTTF(ttfConfig, text);
    textLabel->setPosition(cocos2d::Point(equipWeaponLayer->getContentSize().width * 0.5 + textLabel->getContentSize().width / 2, equipWeaponLayer->getContentSize().height / 2));
    textLabel->setTag(2);// TODO: (kyokomi)とりあえず...サーセン
    equipWeaponLayer->addChild(textLabel);
    
    return equipWeaponLayer;
}




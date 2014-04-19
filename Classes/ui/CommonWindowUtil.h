//
//  CommonWindowUtil.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//

#ifndef __Cocos2dRogueLike__CommonWindowUtil__
#define __Cocos2dRogueLike__CommonWindowUtil__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class CommonWindowUtil
{
public:
    static cocos2d::Point createPointCenter(const cocos2d::Size& baseNodeSize, const cocos2d::Size& targetNodeSize);
    static cocos2d::Point createPointCenterXTopY(const cocos2d::Size& baseNodeSize, const cocos2d::Size& targetNodeSize);
    
    static cocos2d::extension::Scale9Sprite* createWindowWaku();
    static cocos2d::extension::Scale9Sprite* createWindowWaku(cocos2d::Node* pNode);
    static cocos2d::extension::Scale9Sprite* createWindowWaku(cocos2d::Node* pNode, cocos2d::Size padding);
    static cocos2d::extension::Scale9Sprite* createWindowWaku(cocos2d::Size contentSize);
    static void attachWindowWaku(cocos2d::Node* pNode);
    static void attachWindowWaku(cocos2d::Node* pNode, cocos2d::Size padding);
    
    static cocos2d::MenuItemLabel* createMenuItemLabelWaku(cocos2d::Label* pLabel, cocos2d::Size wakuPadding, const cocos2d::ccMenuCallback& callback);
    
    static cocos2d::Layer* createSpriteWithLabelLayer(const cocos2d::Size& layerSize, const std::string& spriteFrameFileName, const cocos2d::TTFConfig& ttfConfig, const std::string& text);
};

#endif /* defined(__Cocos2dRogueLike__CommonWindowUtil__) */

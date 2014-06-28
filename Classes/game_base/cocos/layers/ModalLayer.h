//
//  ModalLayer.h
//
//  Created by kyokomi on 2014/04/13.
//
//

#ifndef __kyokomi_ModalLayer__
#define __kyokomi_ModalLayer__

#include "cocos2d.h"

class ModalLayer : public cocos2d::LayerColor
{
public:
    ModalLayer();
    virtual ~ModalLayer();
    
    virtual bool init();
    
    virtual bool init(const cocos2d::Color3B& color, const GLubyte& opacity);
    static ModalLayer* create(const cocos2d::Color3B& color = cocos2d::Color3B::BLACK, const GLubyte& opacity = 192);
    static ModalLayer* createWithAttachCenterPosition(Node* attachNode, const cocos2d::Color3B& color = cocos2d::Color3B::BLACK, const GLubyte& opacity = 192);
	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
};

#endif /* defined(__kyokomi_ModalLayer__) */

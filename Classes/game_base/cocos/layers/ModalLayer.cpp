//
//  ModalLayer.cpp
//
//  Created by kyokomi on 2014/01/07.
//
//

#include "ModalLayer.h"

USING_NS_CC;

ModalLayer::ModalLayer()
{
}
ModalLayer::~ModalLayer()
{
}

// モーダルレイヤー生成し、中心に配置
ModalLayer* ModalLayer::createWithAttachCenterPosition(Node* attachNode, const cocos2d::Color3B& color, const GLubyte& opacity)
{
    auto modalLayer = ModalLayer::create(color, opacity);
    attachNode->setPosition(Point(modalLayer->getContentSize().width / 2 - attachNode->getContentSize().width / 2,
                                  modalLayer->getContentSize().height / 2 - attachNode->getContentSize().height / 2));
    modalLayer->addChild(attachNode);
    return modalLayer;
}

ModalLayer* ModalLayer::create(const cocos2d::Color3B& color /* = cocos2d::Color3B::BLACK */, const GLubyte& opacity /* = 192 */) {
    ModalLayer *pRet = new ModalLayer();
    if (pRet && pRet->init(color, opacity)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool ModalLayer::init(const cocos2d::Color3B& color, const GLubyte& opacity) {
    if (!LayerColor::init()) {
        return false;
    }
    
    // TouchEvent settings
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(ModalLayer::onTouchBegan, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    this->setColor(color);
    this->setOpacity(opacity);
    
    return true;
}

bool ModalLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (this->isVisible()) {
        // モーダル以降のイベントを強制停止する
        return true;
    }
    return false;
}

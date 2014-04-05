//
//  Joystick.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/02.
//
//

// 製作中
// TODO: 円のpositionがうまく指定できない
// TODO: スティックの移動をスムーズにできない

#include "Joystick.h"

Joystick* Joystick::create() {
    auto ret = new Joystick();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

bool Joystick::init() {
    
    if (Layer::init()) {
        
        enabled_ = true;
        // menu in the center of the screen
        Size s = Director::getInstance()->getWinSize();
        
        this->ignoreAnchorPointForPosition(true);
        setAnchorPoint(Point(0.5f, 0.5f));
        this->setContentSize(s);
        
        setPosition(Point(s.width/2, s.height/2));
        
        // TODO: status
//        _state = Menu::State::WAITING;
        
        // enable cascade color and opacity on menus
        setCascadeColorEnabled(true);
        setCascadeOpacityEnabled(true);
        
        
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(true);
        
        touchListener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);
        touchListener->onTouchCancelled = CC_CALLBACK_2(Joystick::onTouchCancelled, this);
        
        this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
        
        return true;
    }
    return false;
}

bool Joystick::onTouchBegan(Touch* touch, Event* event) {
    
    Point touch_location = this->convertToNodeSpace(touch->getLocation());
    
    // タッチ位置を覚える
    touch_start_point_ = touch_location;
    
    // DrawNode表示（小さい）
    auto draw = dynamic_cast<DrawNode*>(getChildByTag(1));
    if (draw) {
        draw->setVisible(true);
        draw->setPosition(Point(touch_location.x - draw->getContentSize().width/2, touch_location.y - draw->getContentSize().height/2));
    } else {
        draw = DrawNode::create();
        draw->drawDot(touch_location, 20.0, Color4F::WHITE);
        draw->setPosition(Point(touch_location.x - draw->getContentSize().width, touch_location.y - draw->getContentSize().height));
        this->addChild(draw, 1, 1);
    }
    
    // 0.N秒で入力モード（大きくなる）
    
    return true;
}

void Joystick::onTouchEnded(Touch* touch, Event* event) {
    
    // DrawNode非表示
    auto draw = getChildByTag(1);
    if (draw) {
        draw->setVisible(false);
    }
    
    // 覚えていたタッチ位置を初期化
    touch_start_point_ = Point::ZERO;
}
void Joystick::onTouchCancelled(Touch* touch, Event* event) {

    // DrawNode非表示
    auto draw = getChildByTag(1);
    if (draw) {
        draw->setVisible(false);
    }
    
    // 覚えていたタッチ位置を初期化
    touch_start_point_ = Point::ZERO;
}
void Joystick::onTouchMoved(Touch* touch, Event* event) {
    
    Point touch_location = this->convertToNodeSpace(touch->getLocation());
    
    // 移動可能範囲の調整
    if (touch_start_point_.getDistance(touch_location) < 30.0) {
    
        // DrawNode移動
        auto draw = getChildByTag(1);
        if (draw) {
            draw->setPosition(Point(touch_location.x - draw->getContentSize().width, touch_location.y - draw->getContentSize().height));
        }
    }
    
    // N距離でx,y座標に応じたコマンド実行
    
    // 0.N秒待機で連続コマンド実行

}

Joystick::~Joystick() {
    
}

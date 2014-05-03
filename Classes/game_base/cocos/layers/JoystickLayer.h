//
//  JoystickLayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/02.
//
//

#ifndef __Cocos2dRogueLike__JoystickLayer__
#define __Cocos2dRogueLike__JoystickLayer__

#include "cocos2d.h"

USING_NS_CC;

class JoystickLayer : public Layer
{
public:
    
    /** creates an empty Menu */
    static JoystickLayer* create();
    
    virtual bool isEnabled() const { return enabled_; }
    virtual void setEnabled(bool value) { enabled_ = value; };

    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);

protected:
    JoystickLayer(): touch_start_point_(Point::ZERO) {}
    virtual ~JoystickLayer();
    
    bool init();
    
    bool enabled_;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(JoystickLayer);
    
    Point touch_start_point_;
};

#endif /* defined(__Cocos2dRogueLike__JoystickLayer__) */

//
//  Joystick.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/02.
//
//

#ifndef __Cocos2dRogueLike__Joystick__
#define __Cocos2dRogueLike__Joystick__

#include "cocos2d.h"

USING_NS_CC;

class Joystick : public Layer
{
public:
    
    /** creates an empty Menu */
    static Joystick* create();
    
    virtual bool isEnabled() const { return enabled_; }
    virtual void setEnabled(bool value) { enabled_ = value; };

    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);

protected:
    Joystick(): touch_start_point_(Point::ZERO) {}
    virtual ~Joystick();
    
    bool init();
    
    bool enabled_;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(Joystick);
    
    Point touch_start_point_;
};

#endif /* defined(__Cocos2dRogueLike__Joystick__) */

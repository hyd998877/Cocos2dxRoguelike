/**

@file KeypadLayout.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/29

*/

#ifndef __Cocos2dRogueLike__KeypadLayout__
#define __Cocos2dRogueLike__KeypadLayout__

#include "cocos2d.h"
#include <chrono>

/**
@class KeypadLayout KeypadLayout.h

@brief description

detail description

@author kyokomi
@date 2014/06/29
*/
class KeypadLayout : public cocos2d::Node
{
    
public:
    enum ButtonType {
        UP = 0,
        LEFT,
        RIGHT,
        DOWN,
        A,
        B,
        C,
        D,
        SIZE
    };
    typedef std::function<void(void)> KeypadCallback;
    
    KeypadLayout();
    virtual ~KeypadLayout();
    virtual bool init();
    CREATE_FUNC(KeypadLayout);
    
    void setKeypadCallback(ButtonType buttonType, const KeypadCallback& callback);
    
    void showKeypadMenu();
    void hideKeypadMenu();
    bool isKeypadDashed();
    
protected:
    
private:
    cocos2d::Node* initLayout();
    
    cocos2d::Node* _baseLayout;
    
    std::vector<KeypadCallback> _keypadCallbacks;
    
    std::chrono::time_point<std::chrono::system_clock> _touchStartTime;
    
    bool _longTouch;
};

#endif /* defined(__Cocos2dRogueLike__KeypadLayout__) */

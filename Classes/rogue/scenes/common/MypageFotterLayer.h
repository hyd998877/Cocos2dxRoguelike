/**

@file MypageFotterLayer.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#ifndef __Cocos2dRogueLike__MypageFotterLayer__
#define __Cocos2dRogueLike__MypageFotterLayer__

#include "cocos2d.h"

/**
@class MypageFotterLayer MypageFotterLayer.h

@brief description

detail description

@author kyokomi
@date 2014/06/21
*/
class MypageFotterLayer : public cocos2d::Layer
{
    
public:
    MypageFotterLayer();
    virtual ~MypageFotterLayer();
    virtual bool init();
    CREATE_FUNC(MypageFotterLayer);
    
    cocos2d::Size getBaseLayoutSize();
    void setLockMenu(int menuId);
    
protected:
    
private:
    cocos2d::ui::Widget* _baseLayout;

};

#endif /* defined(__Cocos2dRogueLike__MypageFotterLayer__) */

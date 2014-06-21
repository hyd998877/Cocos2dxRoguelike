/**

@file MypageHeaderLayer.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#ifndef __Cocos2dRogueLike__MypageHeaderLayer__
#define __Cocos2dRogueLike__MypageHeaderLayer__

#include "cocos2d.h"

/**
@class MypageHeaderLayer MypageHeaderLayer.h

@brief description

detail description

@author kyokomi
@date 2014/06/21
*/
class MypageHeaderLayer : public cocos2d::Layer
{
    
public:
    MypageHeaderLayer();
    virtual ~MypageHeaderLayer();
    virtual bool init();
    CREATE_FUNC(MypageHeaderLayer);
    
    cocos2d::Size getBaseLayoutSize();
    void setTitleText(const std::string& titleName);
    
protected:
    
private:
    cocos2d::ui::Widget* _baseLayout;

};

#endif /* defined(__Cocos2dRogueLike__MypageHeaderLayer__) */

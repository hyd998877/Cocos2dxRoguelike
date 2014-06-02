//
//  MypageHeaderLayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/07.
//
//

#ifndef __Cocos2dRogueLike__MypageHeaderLayer__
#define __Cocos2dRogueLike__MypageHeaderLayer__

#include "cocos2d.h"

class MypageHeaderLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(MypageHeaderLayer);
    MypageHeaderLayer();
    virtual ~MypageHeaderLayer();
    
    virtual bool init();
    void setTitleText(const std::string &titleText);
private:
    cocos2d::Label* _titleTextLabel;
};

#endif /* defined(__Cocos2dRogueLike__MypageHeaderLayer__) */

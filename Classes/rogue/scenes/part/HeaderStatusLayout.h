/**

@file HeaderStatusLayout.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/07/05

*/

#ifndef __Cocos2dRogueLike__HeaderStatusLayout__
#define __Cocos2dRogueLike__HeaderStatusLayout__

#include "cocos2d.h"
#include "ActorDto.h"

/**
@class HeaderStatusLayout HeaderStatusLayout.h

@brief description

detail description

@author kyokomi
@date 2014/07/05
*/
class HeaderStatusLayout : public cocos2d::Node
{
    
public:
    HeaderStatusLayout();
    virtual ~HeaderStatusLayout();
    virtual bool init();
    CREATE_FUNC(HeaderStatusLayout);
    
    void setStatus(int floorCount, const ActorDto& actorDto, int gold);
    void setEquip(const ActorDto& actorDto);
    void setStatusColor(cocos2d::Color3B color);
protected:
    
private:
    cocos2d::Node* initLayout();
    
    cocos2d::Node* _baseLayout;
};

#endif /* defined(__Cocos2dRogueLike__HeaderStatusLayout__) */

/**

@file BankScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#ifndef __Cocos2dRogueLike__BankScene__
#define __Cocos2dRogueLike__BankScene__

#include "cocos2d.h"

/**
@class BankScene BankScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/15
*/
class BankScene : public cocos2d::Layer
{
    
public:
    BankScene();
    virtual ~BankScene();
    virtual bool init();
    CREATE_FUNC(BankScene);

    static cocos2d::Scene* scene();
protected:
    
private:

};

#endif /* defined(__Cocos2dRogueLike__BankScene__) */

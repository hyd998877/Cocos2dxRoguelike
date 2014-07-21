/**

@file StartScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/07/22

*/

#ifndef __Cocos2dRogueLike__StartScene__
#define __Cocos2dRogueLike__StartScene__

#include "cocos2d.h"

/**
@class StartScene StartScene.h

@brief description

detail description

@author kyokomi
@date 2014/07/22
*/
class StartScene : public cocos2d::Node
{
    
public:
    StartScene();
    virtual ~StartScene();
    virtual bool init();
    CREATE_FUNC(StartScene);
    
protected:
    
private:

};

#endif /* defined(__Cocos2dRogueLike__StartScene__) */

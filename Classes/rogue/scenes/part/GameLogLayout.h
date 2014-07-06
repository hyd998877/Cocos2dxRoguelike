/**

@file GameLogLayout.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/07/06

*/

#ifndef __Cocos2dRogueLike__GameLogLayout__
#define __Cocos2dRogueLike__GameLogLayout__

#include "cocos2d.h"

/**
@class GameLogLayout GameLogLayout.h

@brief description

detail description

@author kyokomi
@date 2014/07/06
*/
class GameLogLayout : public cocos2d::Node
{
    
public:
    GameLogLayout();
    virtual ~GameLogLayout();
    virtual bool init();
    CREATE_FUNC(GameLogLayout);
    
    void logMessage(const char * pszFormat, ...);
protected:
    
private:
    static const int MAX_LOG_LENGTH = 16*1024;
    
    cocos2d::LayerColor* _baseLayer;
    cocos2d::Label* _logTextLabel;
};

#endif /* defined(__Cocos2dRogueLike__GameLogLayout__) */

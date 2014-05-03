//
//  ItemLogic.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#ifndef __Cocos2dxSRPGQuest__ItemLogic__
#define __Cocos2dxSRPGQuest__ItemLogic__

#include "cocos2d.h"
#include "ActorSprite.h"
#include "DropItemSprite.h"

class ItemLogic
{
private:
    
public:
    static std::string use(int itemId, ActorDto* pUseActorDto);
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemLogic__) */

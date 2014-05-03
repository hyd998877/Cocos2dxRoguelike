//
//  ItemLogic.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#ifndef __Cocos2dxSRPGQuest__ItemLogic__
#define __Cocos2dxSRPGQuest__ItemLogic__

#include <string.h>
#include <string>

#include "ActorDto.h"

class ItemLogic
{
private:
    
public:
    static std::string use(int itemId, ActorDto* pUseActorDto);
    static std::string createItemImageFileName(int itemResId);
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemLogic__) */

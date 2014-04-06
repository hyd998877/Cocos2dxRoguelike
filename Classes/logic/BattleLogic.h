//
//  BattleLogic.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#ifndef __Cocos2dxSRPGQuest__BattleLogic__
#define __Cocos2dxSRPGQuest__BattleLogic__

#include <iostream>
#include "ActorSprite.h"

class BattleLogic
{
private:
    
public:
    static int exec(const ActorSprite::ActorDto& from, const ActorSprite::ActorDto& to);
};

#endif /* defined(__Cocos2dxSRPGQuest__BattleLogic__) */

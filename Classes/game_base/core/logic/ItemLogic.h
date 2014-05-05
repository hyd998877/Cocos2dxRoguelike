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

class ItemDto;

class ItemLogic
{
private:
    
public:
    static std::string use(int itemId, ActorDto* pUseActorDto);
    static int sale(const ItemDto &itemDto);
    static std::string createItemImageFileName(int itemResId);
    static int calcMixedItemGold(const ItemDto &baseItemDto, const ItemDto &materialItemDto);
    static bool isMixedItemEvolution(const ItemDto &baseItemDto, const ItemDto &materialItemDto);
    static bool isMixedItem(const ItemDto &baseItemDto, const ItemDto &materialItemDto);
    static ItemDto mixedItem(const ItemDto &baseItemDto, const ItemDto &materialItemDto);
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemLogic__) */

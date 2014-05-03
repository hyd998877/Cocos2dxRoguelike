//
//  ItemInventoryWindowHelper.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#ifndef __Cocos2dRogueLike__ItemInventoryWindowHelper__
#define __Cocos2dRogueLike__ItemInventoryWindowHelper__

#include "ModalLayer.h"
#include "ItemWindowLayer.h"

#include "IteminventoryDto.h"

class ItemInventoryWindowHelper {
public:
    typedef std::list<ItemInventoryDto> ItemInventoryList;
    static ModalLayer* create(ItemInventoryDto itemInventoryDto);
    static ModalLayer* create(ItemInventoryList itemInventoryList);
    
    enum class ActionType {
        ITEM_DROP,
        ITEM_DIST,
        ITEM_USE,
        ITEM_EQUIP,
        ITEM_SALE,
    };
private:
    constexpr static const float WINDOW_SIZE_SCALE = 0.8;
};
#endif /* defined(__Cocos2dRogueLike__ItemInventoryWindowHelper__) */

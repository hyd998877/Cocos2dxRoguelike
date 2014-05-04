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
    enum class ActionType {
        ITEM_DROP,
        ITEM_DIST,
        ITEM_USE,
        ITEM_EQUIP,
        ITEM_SALE,
    };
    
    typedef struct _ActionCallback {
        ActionType _actionType;
        ItemWindowLayer::ItemWindowMenuCallback callback;
    } ActionCallback;
    
    typedef std::list<ItemInventoryDto> ItemInventoryList;
    
    static ModalLayer* create(ItemInventoryDto itemInventoryDto, std::list<ActionCallback> actionCallbackList);
    static ModalLayer* create(ItemInventoryList itemInventoryList, std::list<ActionCallback> actionCallbackList);

private:
    static void searchCallbackFire(std::list<ActionCallback> actionCallbackList, cocos2d::Ref* ref, ItemDto itemDto, ActionType fireActionType);
    
    constexpr static const float WINDOW_SIZE_SCALE = 0.8;
};
#endif /* defined(__Cocos2dRogueLike__ItemInventoryWindowHelper__) */

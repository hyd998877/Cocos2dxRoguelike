/**
 @file ItemStockPageLayer.h
 
 @brief 倉庫画面
 
 倉庫のアイテムと所持アイテムの交換。
 アイテムを捨てたり売ったり整理できます。
 
 @author kyokomi
 @date 2014/05/07
 
 */

#ifndef __Cocos2dRogueLike__ItemStockPageLayer__
#define __Cocos2dRogueLike__ItemStockPageLayer__

#include "cocos2d.h"

#include "ItemInventoryDto.h"

class ItemInventoryLayer;

/**
 @class ItemStockPageLayer ItemStockPageLayer.h
 
 @brief マイページの倉庫ページ用Layerクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class ItemStockPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(ItemStockPageLayer);
    ItemStockPageLayer();
    virtual ~ItemStockPageLayer();
    
    virtual bool init();
private:
    void showItemInventory();
    std::list<ItemInventoryDto> loadItemInventory();
    void saveInventory();
    ItemInventoryDto* findTargetInventory(long objectId);
    void refreshItemInventory(ItemInventoryLayer* itemWindow, const ItemInventoryDto &targetInventory);
    ItemInventoryDto* changeInventory(long objectId);
    
    enum ZOrders {
        DIALOG = 10
    };
    ItemInventoryDto _itemInventory;
    ItemInventoryDto _itemInventoryStock;
};

#endif /* defined(__Cocos2dRogueLike__ItemStockPageLayer__) */

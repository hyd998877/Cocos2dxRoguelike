/**
 @file ItemMixedPageLayer.h
 
 @brief アイテム合成画面
 
 アイテムの合成屋
 
 @author kyokomi
 @date 2014/05/07
 
 */

#ifndef __Cocos2dRogueLike__ItemMixedPageLayer__
#define __Cocos2dRogueLike__ItemMixedPageLayer__

#include "cocos2d.h"

#include "ItemInventoryDto.h"

/**
 @class ItemMixedPageLayer ItemMixedPageLayer.h
 
 @brief マイページのアイテム合成ページのクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class ItemMixedPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(ItemMixedPageLayer);
    ItemMixedPageLayer();
    virtual ~ItemMixedPageLayer();
    
    virtual bool init();
private:
    void showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback);
    bool mixedItem();
    void loadInventory();
    void saveInventory();
    
    void setBaseItem(const ItemDto &itemDto);
    void setMaterialItem(const ItemDto &itemDto);
    void refreshSelectItem(cocos2d::Sprite* materialSprite, cocos2d::Label* label, const ItemDto &itemDto);
    
    enum ZOrders {
        DIALOG = 10
    };
    
    ItemInventoryDto _itemInventory;
    ItemDto _baseItemDto;
    ItemDto _materialItemDto;
};

#endif /* defined(__Cocos2dRogueLike__ItemMixedPageLayer__) */

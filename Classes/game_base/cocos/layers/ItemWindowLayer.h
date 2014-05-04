//
//  ItemWindowLayer.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#ifndef __Cocos2dxSRPGQuest__ItemWindowLayer__
#define __Cocos2dxSRPGQuest__ItemWindowLayer__

#include "cocos2d.h"
#include "ItemDto.h"
#include "ItemInventoryDto.h"

class ItemWindowLayer : public cocos2d::LayerColor
{
public:
    typedef std::function<void(Ref*, ItemDto)> ItemWindowMenuCallback;
protected:
    enum Tags {
        ItemTableLayerTag  = 10000,
        ItemDetailLayerTag = 20000,
        ItemNameTag        = 21000,
        ItemDetailTag      = 22000,
        ItemParamTag       = 23000,
        ItemDetailMenuTag      = 30000,
        ItemDetailMenuUseTag   = 30001,
        ItemDetailMenuDropTag  = 30002,
        ItemDetailMenuEquipTag = 30003,
        ItemCountLabelTag      = 40000,
    };
public:
    ItemWindowLayer();
    virtual ~ItemWindowLayer();
    
    virtual bool initWithContentSize(cocos2d::Size contentSize);
    static ItemWindowLayer* createWithContentSize(cocos2d::Size contentSize);
    
    void sortWeaponWithAccessory();
    
    void setItemUseMenuCallback(const ItemWindowMenuCallback& itemUseMenuCallback);
    void setItemDropMenuCallback(const ItemWindowMenuCallback& itemDropMenuCallback);
    void setItemEquipMenuCallback(const ItemWindowMenuCallback& itemEquipMenuCallback);
    
    void reloadItemList();
    void setItemInventory(const ItemInventoryDto &itemInventoryDto) { _itemInventoryDto = itemInventoryDto; }
    ItemDto findItem(long itemListIndex) const;
private:
    void setItemDetail(long itemListIndex);
    void setItemDetail(const ItemDto& itemDto);
    
    cocos2d::Menu* initCreateMenu();
    
    cocos2d::Label* createDetailTitleLabel(const Node* base, std::string text, float heightPointProportion);
    cocos2d::Label* createDetailTextLabel(const Node* base, std::string text, float heightPointProportion);
private:
    long show_item_detail_idx_;
    
    ItemInventoryDto _itemInventoryDto;
    ItemWindowMenuCallback item_use_menu_callback_;
    ItemWindowMenuCallback item_drop_menu_callback_;
    ItemWindowMenuCallback item_Equip_Menu_Callback_;
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemWindowLayer__) */

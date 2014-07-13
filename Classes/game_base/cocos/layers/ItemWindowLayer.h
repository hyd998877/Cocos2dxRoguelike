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
    enum ItemWindowMenuType {
        ITEM_DROP = 1,  /// < 捨てる
        ITEM_CHANGE,    /// < 交換
        ITEM_USE,       /// < 使う
        ITEM_EQUIP,     /// < 装備/はずす
        
        ITEM_STOCK,     /// < 倉庫へ/持ち物へ
        ITEM_SALE,      /// < 売る
        
        ITEM_MIXED,     /// < 合成選択
    };
    
    typedef std::function<void(ItemWindowMenuType, Ref*, const ItemDto&)> ItemWindowMenuCallback;
protected:
    enum Tags {
        ItemTableLayerTag  = 10000,
        ItemDetailLayerTag = 20000,
        ItemNameTag        = 21000,
        ItemDetailTag      = 22000,
        ItemParamTag       = 23000,
        ItemCountLabelTag      = 40000,
    };
public:
    
    ItemWindowLayer();
    virtual ~ItemWindowLayer();

    static ItemWindowLayer* createWithContentSize(cocos2d::Size contentSize);
    bool initWithContentSize(cocos2d::Size contentSize);
    void initCreateMenu(std::list<ItemWindowLayer::ItemWindowMenuType> menuTypeList);
    
    void sortWeaponWithAccessory();
    
    void setItemMenuCallback(const ItemWindowMenuCallback &itemWindowMenuCallback) { _itemMenuCallback = itemWindowMenuCallback; }
    
    void reloadItemList();
    void setItemInventory(const ItemInventoryDto &itemInventoryDto) { _itemInventoryDto = itemInventoryDto; }
    ItemDto findItem(long itemListIndex) const;
private:
    cocos2d::Menu* _menu;
    cocos2d::Map<int, cocos2d::MenuItem*> _menuItemMap;
    
    void setItemDetail(long itemListIndex);
    void setItemDetail(const ItemDto& itemDto);
    
    cocos2d::Label* createDetailTitleLabel(const Node* base, std::string text, float heightPointProportion);
    cocos2d::Label* createDetailTextLabel(const Node* base, std::string text, float heightPointProportion);
private:
    long show_item_detail_idx_;
    
    ItemInventoryDto _itemInventoryDto;
    ItemWindowMenuCallback _itemMenuCallback;
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemWindowLayer__) */

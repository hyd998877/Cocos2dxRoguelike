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

class ItemWindowLayer : public cocos2d::LayerColor
{
public:
    typedef std::function<void(Ref*, ItemDto)> ItemWindowMenuCallback;
protected:
    enum tag {
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
private:
    long show_item_detail_idx_;
    // TODO: ユーザーデータから持ってくるようにする
    std::list<ItemDto> _itemList;
    ItemWindowMenuCallback item_use_menu_callback_;
    ItemWindowMenuCallback item_drop_menu_callback_;
    ItemWindowMenuCallback item_Equip_Menu_Callback_;
public:
    ItemWindowLayer();
    ~ItemWindowLayer();
    
    virtual bool initWithContentSize(cocos2d::Size contentSize);
    static ItemWindowLayer* createWithContentSize(cocos2d::Size contentSize);
    
    cocos2d::Menu* initCreateMenu();
    ItemDto findItem(long itemListIndex) const;
    void addItemList(const ItemDto& itemDto);
    void setItemEquip(long objectId, bool isEquip);
    void reloadItemList();
    void sortItemList();
    
    void setItemUseMenuCallback(const ItemWindowMenuCallback& itemUseMenuCallback);
    void setItemDropMenuCallback(const ItemWindowMenuCallback& itemDropMenuCallback);
    void setItemEquipMenuCallback(const ItemWindowMenuCallback& itemEquipMenuCallback);
    
    void setItemList(const std::list<ItemDto>& itemList) { _itemList = itemList; }
    const std::list<ItemDto>& getItemList() const { return _itemList; }
private:
    void setItemDetail(long itemListIndex);
    void setItemDetail(const ItemDto& itemDto);
    
    cocos2d::Label* createDetailTitleLabel(const Node* base, std::string text, float heightPointProportion);
    cocos2d::Label* createDetailTextLabel(const Node* base, std::string text, float heightPointProportion);
};

#endif /* defined(__Cocos2dxSRPGQuest__ItemWindowLayer__) */

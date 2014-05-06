//
//  ItemInventoryDto.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#ifndef __Cocos2dRogueLike__ItemInventoryDto__
#define __Cocos2dRogueLike__ItemInventoryDto__

#include <string>
#include <list>
#include <algorithm>

#include "ItemDto.h"

#include "StringUtils.h"

class ItemInventoryDto
{
public:
    typedef std::list<ItemDto> ItemList;
    typedef std::function<bool(const ItemDto& o1, const ItemDto& o2)> Comparator;
    
    ItemInventoryDto();
    ItemInventoryDto(std::string name, long gold, int size, ItemList itemList = ItemList());
    virtual ~ItemInventoryDto();
    
    long getGold() const { return _gold; }
    const std::string& getName() const { return _name; }
    ItemList getItemList() const { return _itemList; }
    int getLimitSize() const { return _size; }
    
    void setItemList(ItemList itemList) { _itemList = itemList; }
    
    bool isInventoryByObjectId(long objectId) const;
    ItemDto findByObjectId(long objectId) const;
    void setGold(int gold);
    void addGold(int gold);
    bool addItemDto(const ItemDto &itemDto);
    void removeItemDto(long objectId);
    void itemEquip(long objectId, bool equip);
    
    bool isLimit() const;

    std::string createTabName() const;
    
    void clearItemList();
    void resetAllEquip();
    void sortItemList(Comparator comparator = ItemDto::compare_dropItem_equip);
    long getMaxObjectId() const;
    
    static ItemInventoryDto* changeInventory(long objectId, ItemInventoryDto *inventory1, ItemInventoryDto *inventory2);
    static ItemInventoryDto* findByObjectIdInventory(long objectId, ItemInventoryDto *inventory1, ItemInventoryDto *inventory2);
private:
    ItemDto* findItemDto(long objectId);
    
    std::string _name;
    long _gold;
    int _size;
    ItemList _itemList;
};

#endif /* defined(__Cocos2dRogueLike__ItemInventoryDto__) */

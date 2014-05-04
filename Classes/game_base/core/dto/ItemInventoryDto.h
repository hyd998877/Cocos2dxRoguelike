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

#include "ItemDto.h"

#include "StringUtils.h"

class ItemInventoryDto
{
public:
    typedef std::list<ItemDto> ItemList;
    typedef std::function<bool(const ItemDto& o1, const ItemDto& o2)> Comparator;
    
    ItemInventoryDto();
    ItemInventoryDto(std::string name, int size, ItemList itemList = ItemList());
    virtual ~ItemInventoryDto();
    
    const std::string& getName() const { return _name; }
    const ItemList& getItemList() const { return _itemList; }
    void setItemList(ItemList itemList) { _itemList = itemList; }
    bool addItemDto(const ItemDto &itemDto);
    void removeItemDto(long objectId);
    void itemEquip(long objectId, bool equip);
    ItemDto* findItemDto(long objectId);
    
    bool isLimit() const;

    std::string createTabName() const;
    
    void clearItemList();
    void resetAllEquip();
    void sortItemList(Comparator comparator = ItemDto::compare_dropItem_equip);
    long getMaxObjectId();
private:
    std::string _name;
    int _size;
    ItemList _itemList;
};

#endif /* defined(__Cocos2dRogueLike__ItemInventoryDto__) */

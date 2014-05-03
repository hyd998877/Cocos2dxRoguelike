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

#include "DropItemSprite.h"

#include "StringUtils.h"

class ItemInventoryDto
{
public:
    typedef std::list<DropItemSprite::DropItemDto> ItemList;
    
    ItemInventoryDto(std::string name, int size, ItemList itemList = ItemList());
    virtual ~ItemInventoryDto();
    
    const std::string& getName() const { return _name; }
    const ItemList& getItemList() const { return _itemList; }
    
    std::string createTabName() const;
    
private:
    std::string _name;
    int _size;
    ItemList _itemList;
};

#endif /* defined(__Cocos2dRogueLike__ItemInventoryDto__) */

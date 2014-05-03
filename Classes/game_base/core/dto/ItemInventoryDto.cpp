//
//  ItemInventoryDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#include "ItemInventoryDto.h"

ItemInventoryDto::ItemInventoryDto(std::string name, int size, ItemList itemList)
: _name(name)
, _size(size)
, _itemList(itemList)
{
    
}

ItemInventoryDto::~ItemInventoryDto()
{
    
}

std::string ItemInventoryDto::createTabName() const {
    return GameCore::StringUtils::format("%s %ld/%d", _name.c_str(), _itemList.size(), _size);
}

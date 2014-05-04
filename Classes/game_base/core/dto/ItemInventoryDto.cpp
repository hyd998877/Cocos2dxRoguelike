//
//  ItemInventoryDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#include "ItemInventoryDto.h"

ItemInventoryDto::ItemInventoryDto()
: _name("")
, _size(0)
, _itemList()
{
}

ItemInventoryDto::ItemInventoryDto(std::string name, int size, ItemList itemList)
: _name(name)
, _size(size)
, _itemList(itemList)
{
}

ItemInventoryDto::~ItemInventoryDto()
{
    
}

std::string ItemInventoryDto::createTabName() const
{
    return GameCore::StringUtils::format("%s %ld/%d",
                                         _name.c_str(),
                                         _itemList.size(),
                                         _size);
}

bool ItemInventoryDto::addItemDto(const ItemDto &itemDto)
{
    if (isLimit()) {
        return false;
    }
    
    this->_itemList.push_back(itemDto);
    return true;
}

ItemDto* ItemInventoryDto::findItemDto(long objectId)
{
    auto it = std::find_if(this->_itemList.begin(), this->_itemList.end(), [objectId](ItemDto itemDto) -> bool {
        if (itemDto.getObjectId() == objectId) {
            return true;
        }
        return false;
    });
    return &(*it);
}
void ItemInventoryDto::removeItemDto(long objectId)
{
    this->_itemList.erase(std::remove_if(this->_itemList.begin(), this->_itemList.end(), [objectId](ItemDto itemDto) -> bool {
        if (itemDto.getObjectId() == objectId) {
            return true;
        }
        return false;
    }), this->_itemList.end());
}

void ItemInventoryDto::itemEquip(long objectId, bool equip)
{
    ItemDto* itemDto = findItemDto(objectId);
    if (itemDto->getObjectId() == objectId) {
        itemDto->setEquip(equip);
    }
}

bool ItemInventoryDto::isLimit() const
{
    if (this->_itemList.size() >= this->_size) {
        return true;
    }
    
    return false;
}

void ItemInventoryDto::clearItemList()
{
    this->_itemList.clear();
}

void ItemInventoryDto::resetAllEquip()
{
    auto it = this->_itemList.begin();
    while (it != this->_itemList.end()) {
        (*it).setEquip(false);
        it++;
    }
}

void ItemInventoryDto::sortItemList(Comparator comparator)
{
    this->_itemList.sort(comparator);
}

long ItemInventoryDto::getMaxObjectId()
{
    auto it = std::max_element(this->_itemList.begin(), this->_itemList.end(), ItemDto::compare_objectId);
    
    return (*it).getObjectId();
}


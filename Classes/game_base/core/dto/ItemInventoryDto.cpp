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
, _gold(0)
, _size(0)
, _itemList()
{
}

ItemInventoryDto::ItemInventoryDto(std::string name, long gold, int size, ItemList itemList)
: _name(name)
, _gold(gold)
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

void ItemInventoryDto::addGold(int gold)
{
    this->_gold += gold;
    if (this->_gold < 0) {
        this->_gold = 0;
    }
}

bool ItemInventoryDto::addItemDto(const ItemDto &itemDto)
{
    if (isLimit()) {
        return false;
    }
    
    this->_itemList.push_back(itemDto);
    return true;
}

bool ItemInventoryDto::isInventoryByObjectId(long objectId) const
{
    auto itemDto = findByObjectId(objectId);
    if (itemDto.getObjectId() == objectId) {
        return true;
    }
    return false;
}

ItemDto ItemInventoryDto::findByObjectId(long objectId) const
{
    if (this->_itemList.size() < 1) {
        return ItemDto();
    }
    auto it = std::find_if(this->_itemList.cbegin(), this->_itemList.cend(), [objectId](ItemDto itemDto) -> bool {
        if (itemDto.getObjectId() == objectId) {
            return true;
        }
        return false;
    });
    if (it == this->_itemList.cend()) {
        return ItemDto();
    }
    return *it;
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
    if (itemDto && itemDto->getObjectId() == objectId) {
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

long ItemInventoryDto::getMaxObjectId() const
{
    auto it = std::max_element(this->_itemList.cbegin(), this->_itemList.cend(), ItemDto::compare_objectId);
    
    return (*it).getObjectId();
}

// private
ItemDto* ItemInventoryDto::findItemDto(long objectId)
{
    auto it = std::find_if(this->_itemList.begin(), this->_itemList.end(), [objectId](ItemDto itemDto) -> bool {
        if (itemDto.getObjectId() == objectId) {
            return true;
        }
        return false;
    });
    if (it == this->_itemList.end()) {
        return nullptr;
    }
    return &(*it);
}

////////////////////////////
// static
ItemInventoryDto* ItemInventoryDto::changeInventory(long objectId, ItemInventoryDto *inventory1, ItemInventoryDto *inventory2)
{
    if (inventory1->isInventoryByObjectId(objectId)) {
        inventory2->addItemDto(inventory1->findByObjectId(objectId));
        inventory1->removeItemDto(objectId);
        return inventory1;
    } else if (inventory2->isInventoryByObjectId(objectId)) {
        inventory1->addItemDto(inventory2->findByObjectId(objectId));
        inventory2->removeItemDto(objectId);
        return inventory2;
    }
    return nullptr;
}


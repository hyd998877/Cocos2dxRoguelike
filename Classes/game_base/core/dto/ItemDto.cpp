//
//  ItemDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#include "ItemDto.h"

ItemDto::ItemDto(long               objectId   /* = 0L */,
                 int                itemId     /* = 0 */,
                 MUseItem::ItemType itemType   /* = MUseItem::ItemType::NONE */,
                 int                imageResId /* = 0 */,
                 std::string        name       /* = "" */,
                 bool               equip      /* = false */,
                 int                param      /* = 0) */)
: _objectId(objectId)
, _itemId(itemId)
, _itemType(itemType)
, _imageResId(imageResId)
, _name(name)
, _equip(equip)
, _param(param)
{
    
}

ItemDto::~ItemDto()
{
    
}

std::string ItemDto::itemDtoToString() const
{
    return GameCore::StringUtils::format("%ld,%d,%d,%d,%s,%d,%d",
                                        this->_objectId,
                                        this->_itemId,
                                        this->_itemType,
                                        this->_imageResId,
                                        this->_name.c_str(),
                                        this->_equip,
                                        this->_param
                                        );
}

ItemDto ItemDto::createItemDtoWithString(const std::string& dataString)
{
    std::vector<std::string> dataStringArray = GameCore::StringUtils::split(std::string(dataString));
    if (dataStringArray.size() != 7) {
        return ItemDto();
    }
    int index = 0;
    
    ItemDto itemDto(
        atol(dataStringArray[index++].c_str()),
        atoi(dataStringArray[index++].c_str()),
        static_cast<MUseItem::ItemType>(atoi(dataStringArray[index++].c_str())),
        atoi(dataStringArray[index++].c_str()),
        dataStringArray[index++],
        static_cast<bool>(atoi(dataStringArray[index++].c_str())),
        atoi(dataStringArray[index++].c_str())
    );
    return itemDto;
}

std::string ItemDto::createItemName() const
{
    std::string itemNameText(this->_name);
    if (this->_param > 0) {
        itemNameText.append(GameCore::StringUtils::format(" + %d", this->_param));
    }
    return itemNameText;
}
//
//  MUseItem.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#include "MUseItem.h"

MUseItem::MUseItem(int useItemId, int useItemImageId, ItemType useItemType, std::string useItemName, std::string useItemDetail, int useItemParam)
: _useItemId(useItemId)
, _useItemImageId(useItemImageId)
, _useItemType(useItemType)
, _useItemName(useItemName)
, _useItemDetail(useItemDetail)
, _useItemParam(useItemParam)
{
    
};

MUseItem::~MUseItem()
{
    
}
//
//  ItemLogic.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#include "ItemLogic.h"

#include "MUseItemDao.h"

#include "StringUtils.h"

using namespace GameCore;

// value
// itemType
// message
std::string ItemLogic::use(int itemId, ActorDto* pUseActorDto)
{
    MUseItem mUseItem = MUseItemDao::getInstance()->selectById(itemId);
    if (mUseItem.getUseItemType() == MUseItem::ItemType::HP_RECOVER_VALUE) {
        pUseActorDto->curehitPoint(mUseItem.getUseItemParam());
        return StringUtils::format("%sをつかってHPが%d回復した。", mUseItem.getUseItemName().c_str(), mUseItem.getUseItemParam());
    } else if (mUseItem.getUseItemType() == MUseItem::ItemType::MP_RECOVER_VALUE) {
        pUseActorDto->cureMagicPoint(mUseItem.getUseItemParam());
        return StringUtils::format("%sを食べて満腹度が%d回復した。", mUseItem.getUseItemName().c_str(), mUseItem.getUseItemParam());
    }
    return "";
}

//
//  ItemLogic.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#include "ItemLogic.h"
#include "MUseItemDao.h"

USING_NS_CC;

// value
// itemType
// message
std::string ItemLogic::use(int itemId, ActorSprite::ActorDto* pUseActorDto)
{
    MUseItem mUseItem = MUseItemDao::getInstance()->selectById(itemId);
    
    // 実行結果値
    int execValue = mUseItem.getUseItemParam();
    
    if (mUseItem.getUseItemType() == MUseItem::ItemType::HP_RECOVER_VALUE)
    {
        if (pUseActorDto->hitPoint + mUseItem.getUseItemParam() > pUseActorDto->hitPointLimit)
        {
            execValue = pUseActorDto->hitPointLimit - pUseActorDto->hitPoint;
        }
        pUseActorDto->hitPoint += execValue;
        
        return StringUtils::format("%sをつかってHPが%d回復した。", mUseItem.getUseItemName().c_str(), execValue);
    }
    else if (mUseItem.getUseItemType() == MUseItem::ItemType::MP_RECOVER_VALUE)
    {
        if (pUseActorDto->magicPoint + mUseItem.getUseItemParam() > pUseActorDto->magicPointLimit)
        {
            execValue = pUseActorDto->magicPointLimit - pUseActorDto->magicPoint;
        }
        pUseActorDto->magicPoint += execValue;
        
        return StringUtils::format("%sを食べて満腹度が%d回復した。", mUseItem.getUseItemName().c_str(), execValue);
    }
    return "";
}

//
//  ItemLogic.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//

#include "ItemLogic.h"

USING_NS_CC;

// value
// itemType
// message
std::string ItemLogic::use(int itemId, ActorSprite::ActorDto* pUseActorDto)
{
    std::string itemName;
    // ItemType
    DropItemSprite::ItemType itemType;
    // value
    int value;
    
    // imageId 10064 imageResId 64
    if (itemId == 1)
    {
        // TODO: とりあえず
        itemName = "ポーション";
        // ItemType
        itemType = DropItemSprite::ItemType::HP_RECOVER_VALUE;
        // value
        value = 10;
    }
    else if (itemId == 2)
    {
        // TODO: とりあえず
        itemName = "ぶどう";
        // ItemType
        itemType = DropItemSprite::ItemType::MP_RECOVER_VALUE;
        // value
        value = 30;
    }
    
    // 実行結果値
    int execValue = value;
    
    if (itemType == DropItemSprite::ItemType::HP_RECOVER_VALUE)
    {
        if (pUseActorDto->hitPoint + value > pUseActorDto->hitPointLimit)
        {
            execValue = pUseActorDto->hitPointLimit - pUseActorDto->hitPoint;
        }
        pUseActorDto->hitPoint += execValue;
        
        auto pStr = String::createWithFormat("%sをつかってHPが%d回復した。", itemName.c_str(), execValue);
        return pStr->getCString();
    }
    else if (itemType == DropItemSprite::ItemType::MP_RECOVER_VALUE)
    {
        if (pUseActorDto->magicPoint + value > pUseActorDto->magicPointLimit)
        {
            execValue = pUseActorDto->magicPointLimit - pUseActorDto->magicPoint;
        }
        pUseActorDto->magicPoint += execValue;
        
        auto pStr = String::createWithFormat("%sを食べて満腹度が%d回復した。", itemName.c_str(), execValue);
        return pStr->getCString();
    }
    
    return "";
}

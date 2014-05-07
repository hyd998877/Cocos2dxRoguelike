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
#include "ItemDto.h"

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

int ItemLogic::sale(const ItemDto &itemDto)
{
    // TODO: 金額計算だけ。実際のお金振込とかアイテム削除は別でやる
    return 10;
}

std::string ItemLogic::createItemImageFileName(int itemResId)
{
    return StringUtils::format("icon_set/item_%d.png", itemResId % 10000);
}

bool ItemLogic::isMixedItem(const ItemDto &baseItemDto, const ItemDto &materialItemDto)
{
    if (baseItemDto.getItemType() == materialItemDto.getItemType()) {
        return true;
    }
    return false;
}

bool ItemLogic::isMixedItemEvolution(const ItemDto &baseItemDto, const ItemDto &materialItemDto)
{
    // TODO: 別アイテムへの進化判定はいずれ
    return false;
}

int ItemLogic::calcMixedItemGold(const ItemDto &baseItemDto, const ItemDto &materialItemDto)
{
    // TODO: 定数にするべきかマスタにするべきか（マスタだと別ゲームでも依存しなく使える。定数だとゲームごとに定義しないとだけど管理が楽）
    return ((baseItemDto.getParam() + materialItemDto.getParam()) * 1000) + 5000;
}

ItemDto ItemLogic::mixedItem(const ItemDto &baseItemDto, const ItemDto &materialItemDto)
{
    ItemDto resultItemDto;
    
    if (isMixedItem(baseItemDto, materialItemDto)) {
        
        if (isMixedItemEvolution(baseItemDto, materialItemDto)) {
            // 別アイテムに進化
            
        } else {
            // 普通の強化
            resultItemDto = ItemDto(baseItemDto);
            resultItemDto.setParam(baseItemDto.getParam() + materialItemDto.getParam());
        }
    }
    return resultItemDto;
}

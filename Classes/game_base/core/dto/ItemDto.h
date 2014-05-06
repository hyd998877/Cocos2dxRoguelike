//
//  ItemDto.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/03.
//
//

#ifndef __Cocos2dRogueLike__ItemDto__
#define __Cocos2dRogueLike__ItemDto__

#include <string>
#include <stdlib.h>

#include "MUseItem.h"

class ItemDto
{
public:
    ///////////////////////
    // コンストラクタ、デストラクタ
    ItemDto(long               objectId   = 0L,
            int                itemId     = 0,
            MUseItem::ItemType itemType   = MUseItem::ItemType::NONE,
            int                imageResId = 0,
            std::string        name       = "",
            bool               equip      = false,
            int                param      = 0);
    ItemDto(long objectId, const MUseItem &mUseItem, int param);
    
    virtual ~ItemDto();
    
    static ItemDto createItemDtoWithString(const std::string& dataString);
    
    ///////////////////////
    // setter getter
    
    int getImageResId() const {
		return _imageResId;
	}
	bool isEquip() const {
		return _equip;
	}
	void setEquip(bool equip) {
		_equip = equip;
	}
	int getItemId() const {
		return _itemId;
	}
	MUseItem::ItemType getItemType() const {
		return _itemType;
	}
	const std::string& getName() const {
		return _name;
	}
	long getObjectId() const {
		return _objectId;
	}
	int getParam() const {
		return _param;
	}
	void setParam(int param) {
		_param = param;
	}

    ///////////////////////
    //
    std::string itemDtoToString() const;
    std::string createItemName() const;
    
    ///////////////////////
    // sort
    
    // 武器、防具優先ソート
    static bool const compare_dropItem_weapon_with_accessory(const ItemDto& o1, const ItemDto& o2) {
        // 装備優先
        if (ItemDto::compare_dropItem_equip(o1, o2)) {
            return true;
            // どっちおなじ対応の場合、id昇順
        } else if (o1._itemType == o2._itemType) {
            if (o1._itemId <= o2._itemId) {
                return true;
            } else {
                return false;
            }
        } else if (o1._itemType == MUseItem::ItemType::EQUIP_WEAPON) {
            return true;
        } else if (o1._itemType == MUseItem::ItemType::EQUIP_ACCESSORY && o2._itemType != MUseItem::ItemType::EQUIP_WEAPON) {
            return true;
        } else {
            return false;
        }
    }
    
    // 装備優先 武器優先
    static bool const compare_dropItem_equip(const ItemDto& o1, const ItemDto& o2)
    {
        if (o1._equip && !o2._equip)
        {
            return true;
        }
        else if (o1._equip && o2._equip && o1._itemType == MUseItem::ItemType::EQUIP_WEAPON)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    // objectId
    static bool const compare_objectId(const ItemDto& o1, const ItemDto& o2)
    {
        if (o1._objectId <= o2._objectId) {
            return true;
        } else {
            return false;
        }
    }
    
private:
    /** すべてのアイテムで一意な値 */
    long _objectId;
    /** アイテムを一意に識別するID. */
    int _itemId;
    
    /** アイテムタイプ */
    MUseItem::ItemType _itemType;
    /** アイテムイメージのID. */
    int _imageResId;
    /** アイテム名. */
    std::string _name;
    
    // status
    bool _equip;
    // 装備なら+値 ゴールドはお金
    int _param;
};

#endif /* defined(__Cocos2dRogueLike__ItemDto__) */

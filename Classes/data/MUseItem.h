//
//  MUseItem.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MUseItem__
#define __Cocos2dRogueLike__MUseItem__

#include <iostream>

/**
 "USE_ITEM_ID": 1,
 "USE_ITEM_IMAGE_ID": 10641,
 "USE_ITEM_TYPE": 100,
 "USE_ITEM_NAME": "ポーション[小]",
 "USE_ITEM_DETAIL": "体力が少し回復します。",
 "USE_ITEM_PARAM": 15
 */
class MUseItem
{
public:
    enum ItemType {
        NONE               = 0,
        EQUIP_WEAPON       = 1,
        EQUIP_ACCESSORY    = 2,
        HP_RECOVER_VALUE   = 100,
        HP_RECOVER_PERCENT = 101,
        MP_RECOVER_VALUE   = 200,
        MP_RECOVER_PERCENT = 201,
    };
    
    MUseItem(int useItemId, int useItemImageId, ItemType useItemType, std::string useItemName, std::string useItemDetail, int useItemParam)
    {
        _useItemId = useItemId;
        _useItemImageId = useItemImageId;
        _useItemType = useItemType;
        _useItemName = useItemName;
        _useItemDetail = useItemDetail;
        _useItemParam = useItemParam;
    };
    
    const int getUseItemId() {return _useItemId;}
    const int getUseItemImageId() {return _useItemImageId;}
    const ItemType getUseItemType() {return _useItemType;}
    const std::string getUseItemName() {return _useItemName;}
    const std::string getUseItemDetail() {return _useItemDetail;}
    const int getUseItemParam() {return _useItemParam;}
    
private:
    int _useItemId;
    int _useItemImageId;
    ItemType _useItemType;
    std::string _useItemName;
    std::string _useItemDetail;
    int _useItemParam;
    //    int _hitProb;
    //    int _attackRangeType;
    //    int _attackRangeValue;
    //    int appendType;
    //    int appendValue;
};


#endif /* defined(__Cocos2dRogueLike__MUseItem__) */

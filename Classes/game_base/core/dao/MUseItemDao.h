//
//  MUseItemDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MUseItemDao__
#define __Cocos2dRogueLike__MUseItemDao__

#include <string>
#include <list>

namespace json11 {
    class Json;
}

class MUseItem
{
public:
    enum ItemType {
        NONE               = 0,
        EQUIP_WEAPON       = 1,
        EQUIP_ACCESSORY    = 2,
        GOLD               = 10,
        USE_ITEM           = 20,
        // TODO: (kyokomi)subTypeだなー
        HP_RECOVER_VALUE   = 100,
        HP_RECOVER_PERCENT = 101,
        MP_RECOVER_VALUE   = 200,
        MP_RECOVER_PERCENT = 201,
    };
    
    MUseItem(int useItemId, int useItemImageId, ItemType useItemType, std::string useItemName, std::string useItemDetail, int useItemParam)
    : _useItemId(useItemId)
    , _useItemImageId(useItemImageId)
    , _useItemType(useItemType)
    , _useItemName(useItemName)
    , _useItemDetail(useItemDetail)
    , _useItemParam(useItemParam)
    {
    };
    
    virtual ~MUseItem()
    {
    }
    
    int getUseItemId() const {return _useItemId;}
    int getUseItemImageId() const {return _useItemImageId;}
    ItemType getUseItemType() const {return _useItemType;}
    const std::string& getUseItemName() const {return _useItemName;}
    const std::string& getUseItemDetail() const {return _useItemDetail;}
    int getUseItemParam() const {return _useItemParam;}
    
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

class MUseItemDao
{
public:
    // シングルトン
    static MUseItemDao* getInstance();
    
    const MUseItem& selectById(int useItemId) const;
    
    MUseItemDao();
    virtual ~MUseItemDao();
    void init(json11::Json json);
private:
    std::list<MUseItem> m_useItemList;
    
    void init();
};


#endif /* defined(__Cocos2dRogueLike__MUseItemDao__) */

//
//  MItemPlusLimitGroup.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MItemPlusLimitGroupDao__
#define __Cocos2dRogueLike__MItemPlusLimitGroupDao__

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace json11 {
    class Json;
}

class MItemPlusLimitGroup
{
public:
    MItemPlusLimitGroup()
    : itemPlusLimitGroupId(0)
    , plusValue(0)
    , prob(0)
    {
        
    }
    virtual ~MItemPlusLimitGroup()
    {
        
    }
    
    int itemPlusLimitGroupId;
    int plusValue;
    int prob;
};

class MItemPlusLimitGroupDao
{
public:
    // シングルトン
    static MItemPlusLimitGroupDao* getInstance();
    
    std::list<MItemPlusLimitGroup> selectById(int itemPlusLimitGroupId) const;
    
    MItemPlusLimitGroupDao();
    virtual ~MItemPlusLimitGroupDao();
    void init(json11::Json json);
private:
    std::list<MItemPlusLimitGroup> _mItemPlusLimitGroupList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MItemPlusLimitGroupDao__) */

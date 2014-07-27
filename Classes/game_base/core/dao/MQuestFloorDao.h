//
//  MQuestFloor.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MQuestFloorDao__
#define __Cocos2dRogueLike__MQuestFloorDao__

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace json11 {
    class Json;
}

class MQuestFloor
{
public:
    MQuestFloor()
    : floorId(0)
    , mobCount(0)
    , mobGroupId(0)
    , dropCount(0)
    , dropGroupId(0)
    , goldMin(0)
    , goldMax(0)
    , itemPlusLimitGroupId(0)
    {
        
    }
    virtual ~MQuestFloor()
    {
        
    }
    
    int floorId;
    int mobCount;
    int mobGroupId;
    int dropCount;
    int dropGroupId;
    int goldMin;
    int goldMax;
    int itemPlusLimitGroupId;
};

class MQuestFloorDao
{
public:
    // シングルトン
    static MQuestFloorDao* getInstance();
    
    const MQuestFloor& selectById(int floorId) const;
    
    MQuestFloorDao();
    virtual ~MQuestFloorDao();
    void init(json11::Json json);
private:
    std::list<MQuestFloor> _mQuestFloorList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MQuestFloorDao__) */

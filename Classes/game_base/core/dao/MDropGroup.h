//
//  MDropGroup.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MDropGroup__
#define __Cocos2dRogueLike__MDropGroup__

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace json11 {
    class Json;
}

class MDropGroup
{
public:
    MDropGroup()
    : dropGroupId(0)
    , dropItemType(0)
    , dropItemId(0)
    , prob(0)
    {
        
    }
    virtual ~MDropGroup()
    {
        
    }
    
    int dropGroupId;
    int dropItemType;
    int dropItemId;
    int prob;
};

class MDropGroupDao
{
public:
    // シングルトン
    static MDropGroupDao* getInstance();
    
    const MDropGroup& selectById(int dropGroupId) const;
    
    MDropGroupDao();
    virtual ~MDropGroupDao();
    void init(json11::Json json);
private:
    std::list<MDropGroup> _mDropGroupList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MDropGroup__) */

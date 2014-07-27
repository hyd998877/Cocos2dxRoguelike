//
//  MDropGroup.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MDropGroupDao__
#define __Cocos2dRogueLike__MDropGroupDao__

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
    : dropGroupId(0)   // key
    , dropItemType(0)  // key
    , dropItemId(0)    // key
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
    
    std::list<MDropGroup> selectById(int dropGroupId) const;
    
    MDropGroupDao();
    virtual ~MDropGroupDao();
    void init(json11::Json json);
private:
    std::list<MDropGroup> _mDropGroupList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MDropGroupDao__) */

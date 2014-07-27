//
//  MMobGroup.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MMobGroup__
#define __Cocos2dRogueLike__MMobGroup__

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace json11 {
    class Json;
}

class MMobGroup
{
public:
    MMobGroup()
    : mobGroupId(0)
    , monsterId(0)
    , popProb(0)
    , dropProb(0)
    {
        
    }
    virtual ~MMobGroup()
    {
        
    }
    
    int mobGroupId;
    int monsterId;
    int popProb;
    int dropProb;
};

class MMobGroupDao
{
public:
    // シングルトン
    static MMobGroupDao* getInstance();
    
    const MMobGroup& selectById(int mobGroupId) const;
    
    MMobGroupDao();
    virtual ~MMobGroupDao();
    void init(json11::Json json);
private:
    std::list<MMobGroup> _mMobGroupList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MMobGroup__) */

//
//  MDungeonDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/27.
//
//

#ifndef __Cocos2dRogueLike__MDungeonDao__
#define __Cocos2dRogueLike__MDungeonDao__

#include <iostream>
#include <string>
#include <vector>
#include <list>

namespace json11 {
    class Json;
}

class MDungeon
{
public:
    MDungeon()
    : dungeonKey()
    , dungeonName()
    , floorIds()
    , clearCount(0)
    {
        
    }
    virtual ~MDungeon()
    {
        
    }
    
    std::string dungeonKey;
    std::string dungeonName;
    std::vector<int> floorIds;
    int clearCount;
};

class MDungeonDao
{
public:
    // シングルトン
    static MDungeonDao* getInstance();
    
    const MDungeon& selectById(const std::string& dungeonKey) const;
    
    MDungeonDao();
    virtual ~MDungeonDao();
    void init(json11::Json json);
private:
    std::list<MDungeon> _MDungeonList;
    
    void init();
};


#endif /* defined(__Cocos2dRogueLike__MDungeonDao__) */

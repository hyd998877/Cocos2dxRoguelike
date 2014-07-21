//
//  MMonsterDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/21.
//
//

#ifndef __Cocos2dRogueLike__MMonsterDao__
#define __Cocos2dRogueLike__MMonsterDao__

#include <iostream>
#include <list>

class ActorDto;

namespace json11 {
    class Json;
}


class MMonsterDao
{
public:
    // シングルトン
    static MMonsterDao* getInstance();
    
    const ActorDto& selectById(int monsterId) const;
    
    MMonsterDao();
    virtual ~MMonsterDao();
    void init(json11::Json json);
private:
    std::list<ActorDto> _mMonsterList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MMonsterDao__) */

//
//  MPlayerDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/07/21.
//
//

#ifndef __Cocos2dRogueLike__MPlayerDao__
#define __Cocos2dRogueLike__MPlayerDao__

#include <iostream>
#include <list>

class ActorDto;

namespace json11 {
    class Json;
}


class MPlayerDao
{
public:
    // シングルトン
    static MPlayerDao* getInstance();
    
    const ActorDto& selectById(int monsterId) const;
    
    MPlayerDao();
    virtual ~MPlayerDao();
    void init(json11::Json json);
private:
    std::list<ActorDto> _mMonsterList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MPlayerDao__) */

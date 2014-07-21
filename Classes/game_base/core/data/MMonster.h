//
//  MMonster.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/03/17.
//
//

#ifndef Cocos2dRogueLike_MMonster_h
#define Cocos2dRogueLike_MMonster_h

#include "cocos2d.h"

USING_NS_CC;

namespace m_monster{
    static ValueMap data_{
        {"901", Value("901,901,0,スライム,2,1,1,1,1,10,10,100,100,5,1,0")},
        {"902", Value("902,902,0,兵士,6,5,3,10,10,20,20,100,100,10,1,0")},
        {"903", Value("903,903,0,ようかん,3,10,1,10,1,3,3,100,100,3,1,0")},
        {"904", Value("904,904,0,ウホッ,20,2,8,8,8,8,8,100,100,1,1,0")},
        {"905", Value("905,905,0,殿,8,8,4,30,30,15,15,100,100,4,1,0")},
        {"906", Value("906,906,0,殿様,13,8,6,40,40,30,30,100,100,6,1,0")},
        {"907", Value("907,907,0,イカ娘,5,5,4,15,15,10,10,100,100,2,1,0")},
        {"908", Value("908,908,0,KC,99,99,99,99,99,99,99,100,100,1,1,0")},
    };
}

// ActorDto
//data.playerId,
//data.imageResId,
//data.faceImgId,
//data.name.c_str(),
//data.attackPoint,
//data.defencePoint,
//data.lv,
//data.exp,
//data.nextExp,
//data.hitPoint,
//data.hitPointLimit,
//data.magicPoint,
//data.magicPointLimit,
//data.movePoint,
//data.attackRange,
//data.gold


#endif

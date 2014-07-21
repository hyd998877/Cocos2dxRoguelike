//
//  MPlayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/03/22.
//
//

#ifndef Cocos2dRogueLike_MPlayer_h
#define Cocos2dRogueLike_MPlayer_h

//
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
//data.movePoint, 視野範囲かな
//data.attackRange,
//data.gold
//
namespace m_player{
    static ValueMap data_{
        {"4", Value("4,1015,0,ジニー,5,1,1,0,10,15,15,100,100,1,1,0")},
    };
}

#endif

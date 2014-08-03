//
//  BattleLogic.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//
#include "BattleLogic.h"

#include "AppMacros.h"

#include "ActorDto.h"
#include "ItemDto.h"

int BattleLogic::exec(const ActorDto& from, const ActorDto& to)
{
    // ダメージ = 攻撃力 * (100 - 防御力(%))
    int damage = (from.getAttackPoint() + from.getWeaponEquip().getTotalParam()) * (100 - (to.getDefencePoint() + from.getAccessoryEquip().getTotalParam())) / 100;
    
    // 10%くらい乱数考慮
    int rand_damage = GetRandom(1, damage);
    return damage + (rand_damage / 10);
}

int BattleLogic::itemThrow(const ItemDto& itemDto, const ActorDto& to)
{
    // TODO: #47 アイテム投げたときのダメージ計算
    int damage = 10;
    
    // 10%くらい乱数考慮
    int rand_damage = GetRandom(1, damage);
    return damage + (rand_damage / 10);
}

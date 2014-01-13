//
//  BattleLogic.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "BattleLogic.h"

int BattleLogic::exec(ActorSprite::ActorDto *from, ActorSprite::ActorDto *to)
{
    // ここでの計算は、乱数などは使わない
    // ダメージ = 攻撃力 * (100 - 防御力(%))
    int damage = (from->attackPoint + from->equip.weaponStr) * (100 - (to->defencePoint + to->equip.accessoryDef)) / 100;
    
    // オーバーキル判定
    if (damage > to->hitPoint) {
        damage = to->hitPoint;
    }
    // HPを更新しないほうがいい？
    to->hitPoint -= damage;
    
    return damage;
}
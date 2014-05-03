//
//  ActorDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#include "ActorDto.h"

#include "MLevelDao.h"

#include "StringUtils.h"

using namespace GameCore;

ActorDto::ActorDto(int playerId,
                   int imageResId,
                   int faceImgId,
                   std::string name,
                   int attackPoint,
                   int defencePoint,
                   int lv,
                   int exp,
                   int nextExp,
                   int hitPoint,
                   int hitPointLimit,
                   int magicPoint,
                   int magicPointLimit,
                   int movePoint,
                   int attackRange,
                   ActorEquipDto weaponEquip,
                   ActorEquipDto accessoryEquip,
                   int gold)

:playerId_(playerId)
,imageResId_(imageResId)
,faceImgId_(faceImgId)
,name_(name)
,attackPoint_(attackPoint)
,defencePoint_(defencePoint)
,lv_(lv)
,exp_(exp)
,nextExp_(nextExp)
,hitPoint_(hitPoint)
,hitPointLimit_(hitPointLimit)
,magicPoint_(magicPoint)
,magicPointLimit_(magicPointLimit)
,movePoint_(movePoint)
,attackRange_(attackRange)
,weaponEquip_(weaponEquip)
,accessoryEquip_(accessoryEquip)
,gold_(gold)
{
    
}

std::string ActorDto::actorToString() {
    return StringUtils::format("%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                               playerId_,
                               imageResId_,
                               faceImgId_,
                               name_.c_str(),
                               attackPoint_,
                               defencePoint_,
                               lv_,
                               exp_,
                               nextExp_,
                               hitPoint_,
                               hitPointLimit_,
                               magicPoint_,
                               magicPointLimit_,
                               movePoint_,
                               attackRange_,
                               gold_
                               );
}

ActorDto ActorDto::createActorDto(const std::string data_string)
{
    std::vector<std::string> data_string_array = StringUtils::split(std::string(data_string));
    int index = 0;
    
    if (data_string_array.size() != 16) {
        return ActorDto();
    }
    
    ActorDto actor_dto(
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       data_string_array[index++],
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       ActorEquipDto(),
       ActorEquipDto(),
       atoi(data_string_array[index++].c_str())
    );
    
    return actor_dto;
}

bool ActorDto::growExpAndLevelUpCheck(int addExp) {
    exp_ += addExp;
    
    if (MLevelDao::getInstance()->checkLevelUp(lv_, exp_)) {
        lv_++;
        auto mLevel = MLevelDao::getInstance()->selectById(lv_);
        // パラメータUp
        hitPointLimit_ += mLevel.getGrowHitPoint();
        attackPoint_   += mLevel.getGrowAttackPoint();
        defencePoint_  += mLevel.getGrowDefencePoint();
        
        return true;
    }
    
    return false;
}

void ActorDto::equipWeapon(const long objectId, const int powerUpCount, const MWeapon& mWeapon) {
    weaponEquip_.equip(objectId, powerUpCount, mWeapon);
}
void ActorDto::equipAccessory(const long objectId, const int powerUpCount, const MAccessory& mAccessory) {
    accessoryEquip_.equip(objectId, powerUpCount, mAccessory);
}

void ActorDto::equipReleaseWeapon() {
    weaponEquip_ = ActorEquipDto();
}
void ActorDto::equipReleaseAccessory() {
    accessoryEquip_ = ActorEquipDto();
}

//
//  ActorEquipDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#include "ActorEquipDto.h"
#include "StringUtils.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"

using namespace GameCore;

ActorEquipDto::ActorEquipDto(const long objectId_,
                             const int itemId_,
                             const std::string name_,
                             const int imgResId_,
                             const int param_,
                             const int powerUpCount_)
:objectId_(objectId_)
,itemId_(itemId_)
,name_(name_)
,imgResId_(imgResId_)
,param_(param_)
,powerUpCount_(powerUpCount_)
{
    
}

ActorEquipDto ActorEquipDto::createEquipDto(const std::string data_string) {
    
    std::vector<std::string> data_string_array = StringUtils::split(std::string(data_string));
    if (data_string_array.size() != ActorEquipDto::SplitCount) {
        return ActorEquipDto();
    }
    
    int index = 0;
    ActorEquipDto equipDto(
       atol(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       data_string_array[index++],
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str()),
       atoi(data_string_array[index++].c_str())
    );
    
    return equipDto;
}

std::string ActorEquipDto::actorEquipToString() const {
    return StringUtils::format("%ld,%d,%s,%d,%d,%d",
                               objectId_,
                               itemId_,
                               name_.c_str(),
                               imgResId_,
                               param_,
                               powerUpCount_
                               );
}

void ActorEquipDto::equip(const long objectId, const int powerUpCount, const MWeapon& mWeapon) {
    objectId_ = objectId;
    itemId_ = mWeapon.getWeaponId();
    name_   = mWeapon.getWeaponName();
    imgResId_ = mWeapon.getWeaponImageId();
    param_ = mWeapon.getAttackPoint();
    powerUpCount_ = powerUpCount;
}

void ActorEquipDto::equip(const long objectId, const int powerUpCount, const MAccessory& mAccessory) {
    objectId_ = objectId;
    itemId_ = mAccessory.getAccessoryId();
    name_   = mAccessory.getAccessoryName();
    imgResId_ = mAccessory.getAccessoryImageId();
    param_ = mAccessory.getDefensePoint();
    powerUpCount_ = powerUpCount;
}


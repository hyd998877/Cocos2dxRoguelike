//
//  ActorEquipDto.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#ifndef __Cocos2dRogueLike__ActorEquipDto__
#define __Cocos2dRogueLike__ActorEquipDto__

#include <iostream>
#include "MWeapon.h"
#include "MAccessory.h"

class ActorEquipDto {
    const static int SplitCount = 6;
    
public:
    ActorEquipDto(const long objectId_    = 0L,
                  const int itemId_       = 0,
                  const std::string name_ = "",
                  const int imgResId_     = 0,
                  const int param_        = 0,
                  const int powerUpCount_ = 0);
    
    static ActorEquipDto createEquipDto(const std::string data_string);
    
    const long getObjectId()    const { return objectId_; }
    const int getItemId()       const { return itemId_; }
    const std::string getName() const { return name_; }
    const int getImgResId()     const { return imgResId_; }
    const int getParam()        const { return param_; }
    const int getPowerUpCount() const { return powerUpCount_; }
    const int getTotalParam()   const { return param_ + powerUpCount_; }
    
    std::string actorEquipToString() const;
    
    void equip(const long objectId, const int powerUpCount, const MWeapon& mWeapon);
    void equip(const long objectId, const int powerUpCount, const MAccessory& mAccessory);
    
private:
    long objectId_;
    int itemId_;
    std::string name_;
    int imgResId_;
    int param_;
    int powerUpCount_;
};

#endif /* defined(__Cocos2dRogueLike__ActorEquipDto__) */

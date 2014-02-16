//
//  MAccessory.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MAccessory__
#define __Cocos2dRogueLike__MAccessory__

#include <iostream>

/**
 "ACCESSORY_ID": 1,
 "ACCESSORY_IMAGE_ID": 11040,
 "ACCESSORY_NAME": "木の盾",
 "ACCESSORY_DETAIL": "平凡な木で作られた盾です。",
 "DEFENSE_POINT": 2,
 "APPEND_TYPE": "NULL",
 "APPEND_VALUE": "NULL"
 */
class MAccessory
{
public:
    MAccessory(int accessoryId, int accessoryImageId, std::string accessoryName, std::string accessoryDetail, int defensePoint)
    {
        _accessoryId = accessoryId;
        _accessoryImageId = accessoryImageId;
        _accessoryName = accessoryName;
        _accessoryDetail = accessoryDetail;
        _defensePoint = defensePoint;
    };
    
    const int getAccessoryId() {return _accessoryId;}
    const int getAccessoryImageId() {return _accessoryImageId;}
    const std::string getAccessoryName() {return _accessoryName;}
    const std::string getAccessoryDetail() {return _accessoryDetail;}
    const int getDefensePoint() {return _defensePoint;}
    
private:
    int _accessoryId;
    int _accessoryImageId;
    std::string _accessoryName;
    std::string _accessoryDetail;
    int _defensePoint;
    //    int _hitProb;
    //    int _attackRangeType;
    //    int _attackRangeValue;
    //    int appendType;
    //    int appendValue;
};


#endif /* defined(__Cocos2dRogueLike__MAccessory__) */

//
//  MAccessoryDao.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/16.
//
//

#ifndef __Cocos2dRogueLike__MAccessoryDao__
#define __Cocos2dRogueLike__MAccessoryDao__

#include <iostream>
#include <list>

namespace json11 {
    class Json;
}

class MAccessory
{
public:
    MAccessory(int accessoryId, int accessoryImageId, std::string accessoryName, std::string accessoryDetail, int defensePoint)
    : _accessoryId(accessoryId)
    , _accessoryImageId(accessoryImageId)
    , _accessoryName(accessoryName)
    , _accessoryDetail(accessoryDetail)
    , _defensePoint(defensePoint)
    {
        
    };
    
    virtual ~MAccessory()
    {
        
    }
    
    int getAccessoryId() const {return _accessoryId;}
    int getAccessoryImageId() const {return _accessoryImageId;}
    const std::string& getAccessoryName() const {return _accessoryName;}
    const std::string& getAccessoryDetail() const {return _accessoryDetail;}
    int getDefensePoint() const {return _defensePoint;}
    
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

class MAccessoryDao
{
public:
    // シングルトン
    static MAccessoryDao* getInstance();
    
    const MAccessory& selectById(int accessoryId) const;
    
    MAccessoryDao();
    virtual ~MAccessoryDao();
    
    void init(json11::Json json);
    
private:
    std::list<MAccessory> m_accessoryList;
    
    void init();
};

#endif /* defined(__Cocos2dRogueLike__MAccessoryDao__) */

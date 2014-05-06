/**
 @file SystemDataDto.cpp
 
 @brief システムデータの入れ物
 
 プレイログとかフラグ管理。
 
 @author kyokomi
 @date 2014/05/06
 
 */
#include "SystemDataDto.h"

#include "StringUtils.h"

SystemDataDto::SystemDataDto()
: _gameProgress(0L)
, _gameObjectCount(0L)
{
    
}

SystemDataDto::SystemDataDto(long gameProgress, long gameObjectCount)
: _gameProgress(gameProgress)
, _gameObjectCount(gameObjectCount)
{
    
}

SystemDataDto::~SystemDataDto()
{
    
}

SystemDataDto SystemDataDto::createWithSeparatedString(const std::string& separatedString)
{
    std::vector<std::string> dataStringArray = GameCore::StringUtils::split(std::string(separatedString));
    if (dataStringArray.size() != 2) {
        return SystemDataDto();
    }
    int index = 0;
    
    SystemDataDto dto(
                      atol(dataStringArray[index++].c_str()),
                      atol(dataStringArray[index++].c_str())
                      );
    return dto;
}

std::string SystemDataDto::toSeparatedString() const
{
    return GameCore::StringUtils::format("%ld,%ld",
                                         this->_gameProgress,
                                         this->_gameObjectCount
                                         );
}


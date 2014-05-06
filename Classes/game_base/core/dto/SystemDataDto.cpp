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
, _playScoreMap()
{
    
}

SystemDataDto::SystemDataDto(long gameProgress, long gameObjectCount, PlayScoreMap playScoreMap)
: _gameProgress(gameProgress)
, _gameObjectCount(gameObjectCount)
, _playScoreMap(playScoreMap)
{
    
}

SystemDataDto::~SystemDataDto()
{
    
}

SystemDataDto SystemDataDto::createWithSeparatedString(const std::string& separatedString)
{
    return createWithSeparatedString(separatedString, PlayScoreMap());
}

SystemDataDto SystemDataDto::createWithSeparatedString(const std::string& separatedString, PlayScoreMap playScoreMap)
{
    std::vector<std::string> dataStringArray = GameCore::StringUtils::split(std::string(separatedString));
    if (dataStringArray.size() != 2) {
        return SystemDataDto();
    }
    int index = 0;
    
    SystemDataDto dto(
                      atol(dataStringArray[index++].c_str()),
                      atol(dataStringArray[index++].c_str()),
                      playScoreMap
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


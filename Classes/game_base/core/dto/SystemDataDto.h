/**
 @file SystemDataDto.h
 
 @brief システムデータの入れ物
 
 プレイログとかフラグ管理。
 
 @author kyokomi
 @date 2014/05/06
 
*/
#ifndef __Cocos2dRogueLike__SystemDataDto__
#define __Cocos2dRogueLike__SystemDataDto__

#include <string>
#include <stdlib.h>
#include <vector>
#include <unordered_map>

/**
 @class SystemDataDto.cpp SystemDataDto.h
 @brief システムデータの入れ物

 プレイログとかフラグ管理するDto
 
 @author kyokomi
 @date 2014/05/06
 */
class SystemDataDto
{
public:
    typedef std::unordered_map<std::string, int> PlayScoreMap;
    SystemDataDto();
    SystemDataDto(long gameProgress, long gameObjectCount, PlayScoreMap playScoreMap);
    virtual ~SystemDataDto();
    
    /**
     @brief カンマ区切り文字列からSystemDataDtoを生成します。
     
     @param[in] separatedString カンマ区切り文字列
     @return SystemDataDto
     */
    static SystemDataDto createWithSeparatedString(const std::string& separatedString);
    
    /**
     @brief カンマ区切り文字列からSystemDataDtoを生成します。
     
     @param[in] separatedString カンマ区切り文字列
     @param[in] playScoreMap プレイスコア
     @return SystemDataDto
     */
    static SystemDataDto createWithSeparatedString(const std::string& separatedString, PlayScoreMap playScoreMap);
    
    /**
     @brief SystemDataDtoの内容を元にカンマ区切り文字列を生成します。
     
     @return カンマ区切り文字列
     */
    std::string toSeparatedString() const;

    long getGameProgress() const { return _gameProgress; }
    void setGameProgress(long gameProgress) { _gameProgress = gameProgress; }
    long getGameObjectCount() const { return _gameObjectCount; }
    void setGameObjectCount(long gameObjectCount) { _gameObjectCount = gameObjectCount; }
    PlayScoreMap getPlayScoreMap() const { return _playScoreMap; }
    void setPlayScoreMap(PlayScoreMap playScoreMap) { _playScoreMap = playScoreMap; }
    void putPlayScoreMap(const std::string &key, int score) { _playScoreMap.insert({key, score}); }
private:
    long _gameProgress;     /// > ゲーム進行状況
    long _gameObjectCount;  /// > ゲーム内のオブジェクトカウント
    PlayScoreMap _playScoreMap;
};

#endif /* defined(__Cocos2dRogueLike__SystemDataDto__) */

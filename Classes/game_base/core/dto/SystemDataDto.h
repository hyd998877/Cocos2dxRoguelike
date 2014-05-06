/**
 @file SystemDataDto.h
 
 @brief システムデータの入れ物
 
 プレイログとかフラグ管理。
 
 @author kyokomi
 @date 2014/05/06
 
*/
#ifndef __Cocos2dRogueLike__SystemDataDto__
#define __Cocos2dRogueLike__SystemDataDto__

#include <iostream>

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
    SystemDataDto();
    SystemDataDto(long gameProgress, long gameObjectCount);
    virtual ~SystemDataDto();
    
    /**
     @brief カンマ区切り文字列からSystemDataDtoを生成します。
     
     @param[in] INパラメータ名 説明
     @return SystemDataDto
     */
    static SystemDataDto createWithSeparatedString(const std::string& separatedString);
    
    /**
     @brief SystemDataDtoの内容を元にカンマ区切り文字列を生成します。
     
     @return カンマ区切り文字列
     */
    std::string toSeparatedString() const;

    long getGameProgress() const { return _gameProgress; }
    void setGameProgress(long gameProgress) { _gameProgress = gameProgress; }
    long getGameObjectCount() const { return _gameObjectCount; }
    void setGameObjectCount(long gameObjectCount) { _gameObjectCount = gameObjectCount; }
    
private:
    long _gameProgress;     /// > ゲーム進行状況
    long _gameObjectCount;  /// > ゲーム内のオブジェクトカウント
};

#endif /* defined(__Cocos2dRogueLike__SystemDataDto__) */

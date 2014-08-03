//
//  RoguePlayDto.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/08.
//
//

#ifndef __Cocos2dRogueLike__RoguePlayDto__
#define __Cocos2dRogueLike__RoguePlayDto__

#include "AppMacros.h"

#include <string>
#include <stdlib.h>
#include <vector>
#include <unordered_map>

NS_ROGUE_BEGIN

/**
 @class RoguePlayDto.cpp RoguePlayDto.h
 @brief ローグプレイの入れ物
 
 ローグプレイ情報を格納するDto
 
 @author kyokomi
 @date 2014/05/08
 */
class RoguePlayDto
{
public:
    // 必要ぽい
    static constexpr auto TUTORIAL_KEY    = "TUTORIAL_KEY";
    static constexpr auto MAIN_QUEST_KEY  = "MAIN_QUEST_KEY";
    static constexpr auto MAIN_QUEST2_KEY = "MAIN_QUEST2_KEY";
    static constexpr auto DEEP_QUEST_KEY  = "DEEP_QUEST_KEY";
    
    enum class QuestType {
        TUTORIAL     = 1, /// > 初心者の洞窟
        MAIN_QUEST   = 2, /// > 不思議な洞窟
        MAIN_QUEST2  = 3, /// > 不思議な洞窟（クリア後）
        DEEP_QUEST   = 4  /// > 最果ての洞窟
    };
    
    // ゲームステート
    enum class GameStatus {
        INIT              = 0,
        PLAYER_TURN       = 10,
        PLAYER_ACTION     = 11,
        PLAYER_NO_ACTION  = 12,
        ENEMY_TURN        = 20,
        ENEMY_ACTION      = 21,
        QUEST_CLEAR       = 97,
        ESCAPE            = 98,
        GAME_OVER         = 99,
    };
    
    RoguePlayDto(QuestType questType,    /// > クエスト種類
                 int questId,            /// > クエストID（階数）
                 GameStatus gameStatus,  /// > ゲーム状態
                 int noActionCount,      /// > 未行動カウント(足踏み自動回復とかの）
                 int turnCount,          /// > ターン数
                 int enemyCount          /// > 敵出現数（トータル）)
    );
    RoguePlayDto();
    virtual ~RoguePlayDto();
    
    /**
     @brief カンマ区切り文字列からRoguePlayDtoを生成します。
     
     @param[in] separatedString カンマ区切り文字列
     @return RoguePlayDto
     */
    static RoguePlayDto createWithSeparatedString(const std::string& separatedString);
    
    /**
     @brief RoguePlayDtoの内容を元にカンマ区切り文字列を生成します。
     
     @return カンマ区切り文字列
     */
    std::string toSeparatedString() const;
    
    static const std::string findQuestKey(QuestType questType);
    
    void setQuestId(int questId) { _questId = questId; }
    void setGameStatus(GameStatus gameStatus) { _gameStatus = gameStatus; }
    void setPlayQuest(QuestType questType, int questId) {
        _questType = questType;
        _questId = questId;
    }
    
    void countUpEnemyCount() { _enemyCount++; }
    void countUpTurn() { _turnCount++; }
    void countUpNoAction() { _noActionCount++; }
    void resetNoActionCount() { _noActionCount = 0; }
    
    QuestType getQuestType() const { return _questType; }
    int getQuestId() const { return _questId; }
    GameStatus getGameStatus() const { return _gameStatus; }
    int getNoActionCount() const { return _noActionCount; }
    int getTurnCount() const { return _turnCount; }
    int getEnemyCount() const { return _enemyCount; }
private:
    QuestType _questType;    /// > クエスト種類
    int _questId;            /// > クエストID（階数）
    GameStatus _gameStatus;  /// > ゲーム状態
    int _noActionCount;      /// > 未行動カウント(足踏み自動回復とかの）
    int _turnCount;          /// > ターン数
    int _enemyCount;         /// > 敵出現数（トータル）
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__RoguePlayDto__) */

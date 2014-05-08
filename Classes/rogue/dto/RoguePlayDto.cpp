//
//  RoguePlayDto.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/08.
//
//

#include "RoguePlayDto.h"

#include "StringUtils.h"

#include "MRogueMap.h"

NS_ROGUE_BEGIN

RoguePlayDto::RoguePlayDto(QuestType questType,    /// > クエスト種類
                           int questId,            /// > クエストID（階数）
                           int floorId,            /// > フロアID(マップデータID)
                           GameStatus gameStatus,  /// > ゲーム状態
                           int noActionCount,      /// > 未行動カウント(足踏み自動回復とかの）
                           int turnCount,          /// > ターン数
                           int enemyCount          /// > 敵出現数（トータル
                           )
: _questType(questType)
, _questId(questId)
, _floorId(questId)
, _gameStatus(gameStatus)
, _noActionCount(noActionCount)
, _turnCount(turnCount)
, _enemyCount(enemyCount)
{
    
}

RoguePlayDto::RoguePlayDto()
: _questType(QuestType::TUTORIAL)
, _questId(0)
, _floorId(0)
, _gameStatus(GameStatus::INIT)
, _noActionCount(0)
, _turnCount(0)
, _enemyCount(0)
{
    
}

RoguePlayDto::~RoguePlayDto()
{
    
}

RoguePlayDto RoguePlayDto::createWithSeparatedString(const std::string& separatedString)
{
    std::vector<std::string> dataStringArray = GameCore::StringUtils::split(std::string(separatedString));
    if (dataStringArray.size() != 7) {
        return RoguePlayDto();
    }
    int index = 0;
    
    RoguePlayDto dto(static_cast<QuestType>(atoi(dataStringArray[index++].c_str())),
                     atoi(dataStringArray[index++].c_str()),
                     atoi(dataStringArray[index++].c_str()),
                     static_cast<GameStatus>(atoi(dataStringArray[index++].c_str())),
                     atoi(dataStringArray[index++].c_str()),
                     atoi(dataStringArray[index++].c_str()),
                     atoi(dataStringArray[index++].c_str())
                     );
    return dto;
}

std::string RoguePlayDto::toSeparatedString() const
{
    return cocos2d::StringUtils::format("%d,%d,%d,%d,%d,%d,%d",
                                        this->_questType,
                                        this->_questId,
                                        this->_floorId,
                                        this->_gameStatus,
                                        this->_noActionCount,
                                        this->_turnCount,
                                        this->_enemyCount
                                        );
}


const std::string& RoguePlayDto::findQuestKey(QuestType questType) {
    switch (questType) {
        case QuestType::TUTORIAL:
            return RogueGameConfig::TUTORIAL_KEY;
        case QuestType::MAIN_QUEST:
            return RogueGameConfig::MAIN_QUEST_KEY;
        case QuestType::MAIN_QUEST2:
            return RogueGameConfig::MAIN_QUEST2_KEY;
        case QuestType::DEEP_QUEST:
            return RogueGameConfig::DEEP_QUEST_KEY;
        default:
            return RogueGameConfig::TUTORIAL_KEY;
    }
}

NS_ROGUE_END

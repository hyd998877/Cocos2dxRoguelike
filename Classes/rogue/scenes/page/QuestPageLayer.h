/**
 @file QuestPageLayer.h
 
 @brief クエスト選択画面
 
 @author kyokomi
 @date 2014/05/07
 */

#ifndef __Cocos2dRogueLike__QuestPageLayer__
#define __Cocos2dRogueLike__QuestPageLayer__

#include "cocos2d.h"
#include "AppMacros.h"
#include "RoguePlayDto.h"

NS_ROGUE_BEGIN

/**
 @class QuestPageLayer QuestPageLayer.h
 
 @brief クエストページLayerクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class QuestPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(QuestPageLayer);
    QuestPageLayer();
    virtual ~QuestPageLayer();
    
    virtual bool init();
    
    void setMenuCallback1(const cocos2d::ccMenuCallback& callback) { _menuCallback1 = callback; }
    void setMenuCallback2(const cocos2d::ccMenuCallback& callback) { _menuCallback2 = callback; }
    void setMenuCallback3(const cocos2d::ccMenuCallback& callback) { _menuCallback3 = callback; }
private:
    std::string createQuestName(const RoguePlayDto::QuestType& questType);
private:
    cocos2d::ccMenuCallback _menuCallback1;
    cocos2d::ccMenuCallback _menuCallback2;
    cocos2d::ccMenuCallback _menuCallback3;
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__QuestPageLayer__) */

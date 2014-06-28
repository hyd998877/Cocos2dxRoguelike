/**

@file QuestChoiceScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#ifndef __Cocos2dRogueLike__QuestChoiceScene__
#define __Cocos2dRogueLike__QuestChoiceScene__

#include "MyPageBaseScene.h"
#include "RoguePlayDto.h"

/**
@class QuestChoiceScene QuestChoiceScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/22
*/
class QuestChoiceScene : public MyPageBaseScene
{
    
public:
    QuestChoiceScene();
    virtual ~QuestChoiceScene();
    CREATE_FUNC(QuestChoiceScene);
    
protected:
    const std::string TITLE_NAME = "くえすと選択";
    const int MENU_ID = 1;
    
    cocos2d::Node* initLayout();
    const std::string& getTitleName() { return TITLE_NAME; }
    int getMenuId() { return MENU_ID; }
private:
    void showBadPlayQuest();
    void playQuest(RogueLikeGame::RoguePlayDto::QuestType questType, int questId);
};

#endif /* defined(__Cocos2dRogueLike__QuestChoiceScene__) */

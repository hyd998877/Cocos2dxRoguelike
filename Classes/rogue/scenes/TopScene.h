/**

@file TopScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#ifndef __Cocos2dRogueLike__TopScene__
#define __Cocos2dRogueLike__TopScene__

#include "MyPageBaseScene.h"

/**
@class TopScene TopScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/21
*/
class TopScene : public MyPageBaseScene
{
public:
    TopScene();
    virtual ~TopScene();
    CREATE_FUNC(TopScene);
    
protected:
    
private:
    const std::string TITLE_NAME = "とっぷぺーじ";
    const int MENU_ID = 0;
    
    cocos2d::Node* initLayout();
    const std::string& getTitleName() { return TITLE_NAME; }
    int getMenuId() { return MENU_ID; }
    
    void initQuestSave();
};

#endif /* defined(__Cocos2dRogueLike__TopScene__) */

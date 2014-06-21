/**

@file ItemMixedScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#ifndef __Cocos2dRogueLike__ItemMixedScene__
#define __Cocos2dRogueLike__ItemMixedScene__

#include "MyPageBaseScene.h"

/**
@class ItemMixedScene ItemMixedScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/22
*/
class ItemMixedScene : public MyPageBaseScene
{
    
public:
    ItemMixedScene();
    virtual ~ItemMixedScene();
    CREATE_FUNC(ItemMixedScene);
    
protected:
    
private:
    const std::string TITLE_NAME = "合成屋";
    const int MENU_ID = 4;
    
    cocos2d::Node* initLayout();
    const std::string& getTitleName() { return TITLE_NAME; }
    int getMenuId() { return MENU_ID; }
};

#endif /* defined(__Cocos2dRogueLike__ItemMixedScene__) */

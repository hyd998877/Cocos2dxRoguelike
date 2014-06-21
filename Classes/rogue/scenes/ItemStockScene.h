/**

@file ItemStockScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#ifndef __Cocos2dRogueLike__ItemStockScene__
#define __Cocos2dRogueLike__ItemStockScene__

#include "MyPageBaseScene.h"

/**
@class ItemStockScene ItemStockScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/22
*/
class ItemStockScene : public MyPageBaseScene
{
    
public:
    ItemStockScene();
    virtual ~ItemStockScene();
    CREATE_FUNC(ItemStockScene);
    
protected:
    
private:
    const std::string TITLE_NAME = "倉庫";
    const int MENU_ID = 2;
    
    cocos2d::Node* initLayout();
    const std::string& getTitleName() { return TITLE_NAME; }
    int getMenuId() { return MENU_ID; }
};

#endif /* defined(__Cocos2dRogueLike__ItemStockScene__) */

/**

@file BankScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#ifndef __Cocos2dRogueLike__BankScene__
#define __Cocos2dRogueLike__BankScene__

#include "MyPageBaseScene.h"

/**
@class BankScene BankScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/15
*/
class BankScene : public MyPageBaseScene
{
    
public:
    BankScene();
    virtual ~BankScene();
    CREATE_FUNC(BankScene);

protected:
    
private:
    const std::string TITLE_NAME = "銀行屋";
    const int MENU_ID = 3;
    
    cocos2d::Node* initLayout();
    const std::string& getTitleName() { return TITLE_NAME; }
    int getMenuId() { return MENU_ID; }

};

#endif /* defined(__Cocos2dRogueLike__BankScene__) */

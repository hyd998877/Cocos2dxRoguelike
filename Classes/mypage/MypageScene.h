//
//  MypageScene.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/23.
//
//

#ifndef __Cocos2dRogueLike__MypageScene__
#define __Cocos2dRogueLike__MypageScene__

#include "cocos2d.h"
#include "AppMacros.h"
#include "ItemWindow.h"

USING_NS_CC;
using namespace std;

NS_ROGUE_BEGIN

class MypageScene : public Layer
{
public:
    static Scene* scene();
private:
    MypageScene();
    virtual ~MypageScene();
    
    bool init();
    void initMyPage();

    CREATE_FUNC(MypageScene);
    
    /** グロナビ生成 */
    void initGlobalMenu();
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__MypageScene__) */

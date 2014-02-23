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

USING_NS_CC;
using namespace std;

class MypageScene : public Layer
{
private:
    /** グロナビ生成 */
    void initGlobalMenu();
    
public:
    MypageScene();
    ~MypageScene();
    
    virtual bool init();
    
    static Scene* scene();
    
    CREATE_FUNC(MypageScene);
};

#endif /* defined(__Cocos2dRogueLike__MypageScene__) */

//
//  BattleSecne.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/22.
//
//

#ifndef __Cocos2dRogueLike__BattleSecne__
#define __Cocos2dRogueLike__BattleSecne__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class BattleScene : public Layer
{
public:
    BattleScene();
    ~BattleScene();
    
    virtual bool init();
    
    static Scene* scene();
    
    CREATE_FUNC(BattleScene);
    
    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
};


#endif /* defined(__Cocos2dRogueLike__BattleSecne__) */

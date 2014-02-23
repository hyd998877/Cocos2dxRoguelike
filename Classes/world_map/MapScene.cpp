//
//  MapScene.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/09/29.
//
//

#include "MapScene.h"

#include "SRPGScene.h"
#include "TitleSceneLoader.h"
#include "RogueScene.h"
#include "NovelScene.h"
#include "BattleScene.h"


MapScene::MapScene()
{
    // コンストラクタ
}

MapScene::~MapScene()
{
    // デストラクタ
    this->removeAllChildrenWithCleanup(true);
}

bool MapScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    return true;
}

void MapScene::onEnterTransitionDidFinish()
{
}

SEL_MenuHandler MapScene::onResolveCCBCCMenuItemSelector(cocos2d::Object *pTarget, const char *pSelectorName)
{
    CCLOG("name = %s", pSelectorName);
    
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "tappedMenuItem1", MapScene::tappedMenuItem1);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "tappedMenuItem2", MapScene::tappedMenuItem2);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "tappedMenuItem3", MapScene::tappedMenuItem3);
    
    return NULL;
}


Control::Handler MapScene::onResolveCCBCCControlSelector(cocos2d::Object *pTarget, const char *pSelectorName)
{
    CCLOG("name = %s", pSelectorName);
    return NULL;
}

void MapScene::tappedMenuItem1(Object * pTarget)
{
    CCLOG("tappedMenuItem1");
    Scene* scene = NovelScene::scene(1, 0, [this]() {
        CCLOG("novel end");
        Scene* scene = TitleSceneLoader::scene();
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
    Director::getInstance()->replaceScene(trans);
}

void MapScene::tappedMenuItem2(Object * pTarget)
{
    CCLOG("tappedMenuItem2");
//    Scene* scene = SRPGScene::scene();
//    TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
//    Director::getInstance()->replaceScene(trans);
    Scene* scene = BattleScene::scene();
    TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
    Director::getInstance()->replaceScene(trans);

}

void MapScene::tappedMenuItem3(Object * pTarget)
{
    CCLOG("tappedMenuItem3");
    Scene* scene = NovelScene::scene(2, 0, [this]() {
        CCLOG("novel2 end");
        auto scene = RogueScene::scene(1);
        auto trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
    Director::getInstance()->replaceScene(trans);
}



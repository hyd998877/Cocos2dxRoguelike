/**
 
 @file StartScene.h
 
 @brief description
 
 Cocos2dRogueLike
 
 @author kyokomi
 @date 2014/06/21
 
 */

#ifndef __Cocos2dRogueLike__StartScene__
#define __Cocos2dRogueLike__StartScene__

#include "cocos2d.h"

namespace json11 {
    class Json;
}

/**
 @class StartScene StartScene.h
 
 @brief description
 
 detail description
 
 @author kyokomi
 @date 2014/06/21
 */
class StartScene : public cocos2d::Layer
{
public:
    typedef std::function<void(bool error, json11::Json json)> RequestMasterDataCallback;
    
    StartScene();
    virtual ~StartScene();
    CREATE_FUNC(StartScene);
    
    static cocos2d::Scene* scene() {
        cocos2d::Scene *scene = cocos2d::Scene::create();
        auto layer = StartScene::create();
        scene->addChild(layer);
        return scene;
    }
    
    virtual bool init();
    
protected:
    
private:
    void requestMasterData(RequestMasterDataCallback callback);
};

#endif /* defined(__Cocos2dRogueLike__StartScene__) */

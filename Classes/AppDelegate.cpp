#include "AppDelegate.h"
#include "AppMacros.h"
#include "extensions/cocos-ext.h"

#include "MypageScene.h"
#include "RogueScene.h"

USING_NS_CC;
USING_NS_CC_EXT;


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    Director* director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("Cocos2dRogueLike Game");
        director->setOpenGLView(glview);
    }
    // チラツキ対策
    Director::getInstance()->setDepthTest(false);
  
    // 800x480基準
    glview->setDesignResolutionSize(
                                     800.0f * GAME_SCALE, // 13
                                     480.0f * GAME_SCALE, //  7.8
//                                     32 * 13,// 352 416   704 800
//                                     32 * 7 ,// 224 249.6 448 480
                                     ResolutionPolicy::SHOW_ALL);
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = MypageScene::scene();
    
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();

    // フォアグラウンド時のイベントを発行
//    cocos2d::EventCustom foreground_event_rgoue(EVENT_COME_TO_FOREGROUND_ROGUE);
//    Director::getInstance()->getEventDispatcher()->dispatchEvent(&foreground_event_rgoue);
}

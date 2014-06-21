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

#if 1
    float baseScale = 0.75f;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    int widthDiff = (int)visibleSize.width % (int)480.f;
    int heightDiff = (int)visibleSize.height % (int)320.f;
    CCLOG("############ %f %f wdif = %d hdif = %d", visibleSize.width, visibleSize.height, widthDiff, heightDiff);
    if (widthDiff == 0 && heightDiff == 0) {
        // 800x480基準
        glview->setDesignResolutionSize(
                                        480 * baseScale,
                                        320 * baseScale,
                                        //                                    800.0f * GAME_SCALE, // 13
                                        //                                    480.0f * GAME_SCALE, //  7.8
                                        //                                     32 * 13,// 352 416   704 800
                                        //                                     32 * 7 ,// 224 249.6 448 480
                                        ResolutionPolicy::SHOW_ALL);
    } else {
        if (widthDiff > heightDiff) {
            // 横のほうがずれがでかいので左と右に黒い帯がでる感じ。なんとかする
            float scale = (480.0f * baseScale) / visibleSize.width;
            glview->setDesignResolutionSize(
                                            (480 * baseScale) * (1 + scale),
                                            (320 * baseScale),
                                            ResolutionPolicy::SHOW_ALL);
        } else {
            // 縦のほうがずれがでかいので下と上に黒い帯が出る感じ
            glview->setDesignResolutionSize(
                                            (480 * baseScale),
                                            (320 * baseScale),
                                            ResolutionPolicy::SHOW_ALL);
        }
    }
#else 
    glview->setDesignResolutionSize(
                                    (960 * baseScale),
                                    (640 * baseScale),
                                    ResolutionPolicy::SHOW_ALL);
#endif
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = RogueLikeGame::MypageScene::scene();
    
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

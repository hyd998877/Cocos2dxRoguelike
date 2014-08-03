#include "AppDelegate.h"
#include "AppMacros.h"
#include "extensions/cocos-ext.h"

#include "StartScene.h"

#include "AudioUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;


AppDelegate::AppDelegate()
{

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

    const float baseScale  = 0.75f;
    const float baseWidth  = 480.0f;
    const float baseHeight = 320.0f;
    const float baseScaleWidth  = baseWidth * baseScale;
    const float baseScaleHeight = baseHeight * baseScale;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    const int widthDiff = (int)visibleSize.width % (int)baseWidth;
    const int heightDiff = (int)visibleSize.height % (int)baseHeight;

    if (widthDiff == 0 && heightDiff == 0) {
        glview->setDesignResolutionSize(baseScaleWidth, baseScaleHeight, ResolutionPolicy::SHOW_ALL);
    } else {
        float divX = visibleSize.width / baseWidth;
        float divY = visibleSize.height / baseHeight;
        glview->setDesignResolutionSize(baseScaleWidth + (widthDiff / divX),
                                        baseScaleHeight + (heightDiff / divY),
                                        ResolutionPolicy::SHOW_ALL);
        CCLOG("###### designSize w=%f h=%f", (baseScaleWidth + (widthDiff / divX)), (baseScaleHeight + (heightDiff / divY)));
    }
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = StartScene::scene();
    
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    AudioUtil::pause();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    AudioUtil::resume();
}

#include "AppDelegate.h"
#include "AppMacros.h"
#include "extensions/cocos-ext.h"

#include "StartScene.h"

#include "AudioUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;

void fitDesignResolutionSize(GLView *glview, float width, float height, float scale = 1.00f);

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

    // 480x320準拠
    fitDesignResolutionSize(glview, 480.0f, 320.0f, 0.75f);
    
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

void fitDesignResolutionSize(GLView *glview, float width, float height, float scale)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    const float baseScaleWidth  = width * scale;
    const float baseScaleHeight = height * scale;
    
    const int widthDiff = abs((int)visibleSize.width - (int)width);
    const int heightDiff = abs((int)visibleSize.height - (int)height);
    
    if (widthDiff == 0 && heightDiff == 0) {
        glview->setDesignResolutionSize(baseScaleWidth,
                                        baseScaleHeight,
                                        ResolutionPolicy::SHOW_ALL);
    } else {
        float divX = visibleSize.width / baseScaleWidth;
        float divY = visibleSize.height / baseScaleHeight;
        float fixWidth = widthDiff / divY;
        float fixHeight = heightDiff / divX;
        glview->setDesignResolutionSize(baseScaleWidth + fixWidth,
                                        baseScaleHeight + fixHeight,
                                        ResolutionPolicy::SHOW_ALL);
    }
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

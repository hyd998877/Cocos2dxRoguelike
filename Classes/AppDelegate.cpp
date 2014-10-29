#include "AppDelegate.h"
#include "AppMacros.h"
#include "extensions/cocos-ext.h"

#include "StartScene.h"

#include "AudioUtil.h"

USING_NS_CC;
USING_NS_CC_EXT;

Size getFitDesignResolutionSize(float width, float height, float scale);

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
    auto fitSize = getFitDesignResolutionSize(480.0f, 320.0f, 1.0f);
    CCLOG("fitSize w = %f h = %f", fitSize.width, fitSize.height);
    glview->setDesignResolutionSize(fitSize.width, fitSize.height, ResolutionPolicy::SHOW_ALL);
    
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

Size getFitDesignResolutionSize(float width, float height, float scale)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    const float baseScaleWidth  = width * scale;
    const float baseScaleHeight = height * scale;
    
    const int widthDiff = abs((int)visibleSize.width - (int)width);
    const int heightDiff = abs((int)visibleSize.height - (int)height);
    
    if (widthDiff == 0 && heightDiff == 0) {
        return Size(baseScaleWidth, baseScaleHeight);
    }
    
    float divX = visibleSize.width / baseScaleWidth;
    float divY = visibleSize.height / baseScaleHeight;
    float fixWidth = widthDiff / divY;
    float fixHeight = heightDiff / divX;
    if (width > height) {
        float addWidth = (heightDiff / divY * (width / height));
        return Size(baseScaleWidth + fixWidth - addWidth, baseScaleHeight);
    } else {
        float addheight = (widthDiff / divX * (height / width));
        return Size(baseScaleWidth, baseScaleHeight + fixHeight - addheight);
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

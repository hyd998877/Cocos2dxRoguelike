/**

@file MyPageBaseScene.h

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#ifndef __Cocos2dRogueLike__MyPageBaseScene__
#define __Cocos2dRogueLike__MyPageBaseScene__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

/**
@class MyPageBaseScene MyPageBaseScene.h

@brief description

detail description

@author kyokomi
@date 2014/06/21
*/
class MyPageBaseScene : public cocos2d::Layer
{
    
public:
    MyPageBaseScene();
    virtual ~MyPageBaseScene();
    virtual bool init();
    
    template <class T>
    static cocos2d::Scene* scene() {
        cocos2d::Scene *scene = cocos2d::Scene::create();
        auto layer = T::create();
        scene->addChild(layer);
        return scene;
    }
    
    static cocos2d::ui::Widget* createCocoStudioWidget(const std::string& cocostudioFilePath) {
        return cocostudio::GUIReader::getInstance()->widgetFromJsonFile(cocostudioFilePath.c_str());
    }
    
    cocos2d::Layer* getDialogLayer() { return _dialogLayer; }
    
protected:

private:
    enum ZOrder {
        BODY   =   100,
        HEADER =  1000,
        FOTTER =  1001,
        DIALOG = 10000
    };
    
    virtual cocos2d::Node* initLayout() = 0;
    virtual const std::string& getTitleName() = 0;
    virtual int getMenuId() = 0;
    
    cocos2d::Layer* _dialogLayer;
};

#endif /* defined(__Cocos2dRogueLike__MyPageBaseScene__) */

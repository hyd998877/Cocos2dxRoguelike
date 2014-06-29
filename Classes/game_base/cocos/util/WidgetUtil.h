//
//  WidgetUtil.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/06/21.
//
//

#ifndef Cocos2dRogueLike_WidgetUtil_h
#define Cocos2dRogueLike_WidgetUtil_h

#include "cocos2d.h"

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

class WidgetUtil
{
public:
    static cocos2d::ui::Widget* createCocoStudioWidget(const std::string& cocostudioFilePath) {
        return cocostudio::GUIReader::getInstance()->widgetFromJsonFile(cocostudioFilePath.c_str());
    }
    
    static cocos2d::ui::Widget* getChildByNameRecursion(cocos2d::ui::Widget* rootWidget, std::string name) {
        for (auto& child : rootWidget->getChildren()) {
            if (child) {
                cocos2d::ui::Widget* widgetChild = dynamic_cast<cocos2d::ui::Widget*>(child);
                if (widgetChild) {
                    if (widgetChild->getName() == name) {
                        return widgetChild;
                    } else {
                        auto widgetGrandChild = getChildByNameRecursion(widgetChild, name);
                        if (widgetGrandChild) {
                            return widgetGrandChild;
                        }
                    }
                }
            }
        }
        return nullptr;
    }
    
    static void onTouchEvent(cocos2d::ui::Widget* rootWidget, const std::string& buttonName, const cocos2d::ui::Widget::ccWidgetTouchCallback& callback) {
        auto button = dynamic_cast<cocos2d::ui::Button*>(WidgetUtil::getChildByNameRecursion(rootWidget, buttonName));
        button->addTouchEventListener(callback);
    }
    
    static void onTouchEventEnded(cocos2d::ui::Widget* rootWidget, const std::string& buttonName, const cocos2d::ui::Widget::ccWidgetTouchCallback& callback) {
        onTouchEvent(rootWidget, buttonName, [callback](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
            if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
                callback(ref, type);
            }
        });
    }
    
    static void onTouchEventLongTouch(cocos2d::ui::Widget* rootWidget, const std::string& buttonName, const cocos2d::ui::Widget::ccWidgetTouchCallback& callback) {
        onTouchEvent(rootWidget, buttonName, [rootWidget, callback](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
            int count = 0;
            if (type == cocos2d::ui::Widget::TouchEventType::BEGAN) {
                rootWidget->getScheduler()->schedule([rootWidget, callback, ref, type, &count](float dt) {
                    (count)++;
                    CCLOG("longTouchEvent %f", dt);
                    callback(ref, type);
                }, rootWidget, 0.05, kRepeatForever, 0.3, false, "longTouchEvent");
            } else if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
                rootWidget->getScheduler()->unschedule("longTouchEvent", rootWidget);
                CCLOG("stop longTouchEvent");
                if (count == 0) {
                    callback(ref, type);
                }
            } else if (type == cocos2d::ui::Widget::TouchEventType::CANCELED) {
                rootWidget->getScheduler()->unschedule("longTouchEvent", rootWidget);
                CCLOG("stop longTouchEvent");
            }
        });
    }
    
    static void settingCenterPosition(cocos2d::ui::Widget* widget) {
        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
        widget->setPosition(cocos2d::Vec2(visibleSize.width/2 - widget->getContentSize().width/2*widget->getScaleX(), visibleSize.height/2 - widget->getContentSize().height/2*widget->getScaleY()));
    }
};

#endif
